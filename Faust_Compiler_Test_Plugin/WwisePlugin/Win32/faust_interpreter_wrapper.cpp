#include "faust_interpreter_wrapper.h"
#include <faust/dsp/libfaust.h> 
#include "utils.h"
#include <iostream>
#include <thread>

static inline void normalizePath(std::string& path) {
    std::replace(path.begin(), path.end(), '\\', '/');
}

FaustInterpreterWrapper::FaustInterpreterWrapper(std::string nameApp)
    : factory(nullptr)
    , name_app(nameApp)
    , cppfile("")
{
    faust_includedir = execCommand("faust --includedir");
    faust_dspdir = execCommand("faust --dspdir");

    normalizePath(faust_includedir);
    normalizePath(faust_dspdir);

    archfile = faust_dspdir + "/wwise.cpp";
    // TODO: store in a file maybe?
}

void FaustInterpreterWrapper::set_exportPath(const std::string& path)
{
    exportPath = path;
    normalizePath(exportPath);
}

FaustInterpreterWrapper::~FaustInterpreterWrapper()
{
    if (factory)
    {
        deleteInterpreterDSPFactory(factory);
        factory = nullptr;
    }
}

std::string FaustInterpreterWrapper::get_default_entry_code()
{
    return default_entry_code;
}

bool FaustInterpreterWrapper::previewPlugin(const std::string& dspCode)
{

    // directly create a new dsp factory to exploit its caching ability 
    // const std::string &name_app, const std::string &dsp_content, int argc, const char *argv[], std::string &error_msg)
    // TODO: fix compilation options
    // Define compilation options (none for now, but could add e.g. "-vec")
    // int argc = 0;
    // const char** argv = nullptr;
    const char* argv[] = { 
        "-a", archfile.c_str(),
        "-I", faust_includedir.c_str(), 
        "-I", faust_dspdir.c_str(),
        "-lang", "cpp", 
        "-json"
    };
    int argc = 9;

    std::string errorMessage;
    interpreter_dsp_factory* newFactory = createInterpreterDSPFactoryFromString(
        name_app,
        dspCode,
        argc,
        argv,
        errorMessage
    );

    if (!newFactory) 
    {
        std::cerr << "Faust compilation failed: " << errorMessage << std::endl;
        return false;
    }

    if(factory && factory != newFactory){
        deleteInterpreterDSPFactory(factory);
    }

    //  update states
    factory = newFactory;
    lastDSPCode = dspCode;
    currentSHA = factory->getSHAKey();
    // name_app = factory->getName(); --> @TODO fix bug : Exception thrown at 0x00007FFA23425369 in Wwise.exe: Microsoft C++ exception: faustexception at memory location 0x000000C2BFAFE9C0. Unhandled exception at 0x00007FFA23425369 (KernelBase.dll) in Wwise.exe: 0xC000041D: An unhandled exception was encountered during a user callback.

    bool exported = exportCPP();
    if (!exported)
    {
        std::cerr << "Cpp file could not be exported."<<std::endl;
        return false;
    }
    
    bool compiled = compileCPP();
    if (!compiled)
    {    
        std::cerr << "Cpp file could not be compiled."<<std::endl;
        return false;
    }
    return true;
}

bool FaustInterpreterWrapper::exportCPP() {
    
    tempDir = createTempDir();
    cppfile = tempDir.string() + '/' + name_app + ".cpp";

    if (!factory) {
        std::cerr << "No compiled DSP factory available for export." << std::endl;
        return false;
    }

    if (!writeInterpreterDSPFactoryToBitcodeFile(factory, cppfile)) {
        std::cerr << "Failed to write DSP bytecode to file: " << cppfile << std::endl;
        return false;
    }

    // move the json file 
    moveFile(name_app+".json", tempDir.string() + '/' + name_app + ".json");

    return true;
}

bool FaustInterpreterWrapper::compileCPP()
{
    // @TODO integration dependent path --> will be installed in smth like : faust/architecture/wwise/interpreter/CMakeLists.txt
    std::filesystem::path currFile = __FILE__;  // CMakeLists.txt is located in the same dir
    std::string cmakelists_dir = currFile.parent_path().string(); // dir where the CMakeLists.txt is located
    std::string cmd = 
        "cmake -B \"" + tempDir.string() + "\" "
        "-S \"" + cmakelists_dir + "\" "
        "-DFAUST_INCLUDE_DIR=\"" + faust_includedir + "\" "
        "-DFAUST_LIBRARY=\"" + faust_dspdir + "\" "
        "-DPROJECT_NAME=" + name_app + " "
        "-DFAUST_CPP_FILE=\"" + cppfile + "\" "
        "&& cmake --build \"" + tempDir.string() + "\" --config Release";

    std::cout << "Running: " << cmd << std::endl;

    int result = std::system(cmd.c_str());
    if(result != 0)
    {
        std::cerr << "Build command failed with code " << result << std::endl;
        return false;
    }
    std::cout << "Build succeeded!" << std::endl;
    return true;
}

bool FaustInterpreterWrapper::moveFile(const std::string& sourcePath, const std::string& destPath)
{
    try {
        std::filesystem::path src(sourcePath);
        std::filesystem::path dst(destPath);

        if (!std::filesystem::exists(src)) {
            std::cerr << "Source file does not exist: " << src << '\n';
            return false;
        }

        std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing);
        std::filesystem::remove(src); // Delete original
        return true;
    } 
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        return false;
    }
}

#include <fstream>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <sstream>

bool FaustInterpreterWrapper::buildPlugin(const std::string& dspCode) {

    auto tempDir = createTempDir();

    // write dspCode into the example.dsp file. Name of plugin will be set automatically via declare.
    std::filesystem::path dspPath = tempDir / "example.dsp";
    std::ofstream outFile(dspPath);
    if (!outFile.is_open()) {
        return false;
    }
    outFile << dspCode;
    outFile.close();

    // call faust2wwise inside the temp directory
    std::ostringstream cmd;
    cmd << "cd /d \"" << tempDir.string() << "\" && faust2wwise example.dsp > output.log 2>&1";

    int result = std::system(cmd.str().c_str());

    return result == 0;
}

// TODO: discard timestamp, and use the plugin name for the new directory.
std::filesystem::path FaustInterpreterWrapper::createTempDir()
{
    std::ostringstream timestamp;
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    timestamp << std::put_time(std::localtime(&now_time_t), "%Y%m%d_%H%M%S");

    std::filesystem::path tempDir = std::filesystem::path( exportPath + "/faust_" + timestamp.str());

    try {
        std::filesystem::create_directories(tempDir);
    } catch (const std::filesystem::filesystem_error& e) {
        // handle failure to create directory
        return false;
    }
    return tempDir;
}