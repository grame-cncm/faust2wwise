#include "interpreter_wrapper.h"
#include "PluginUtils/syscall.h"
#include <faust/dsp/libfaust.h> 
#include <iostream>

#define STORAGE_DIRNAME "_f2wTemp_"

static inline void normalizePath(std::string& path) {
    std::replace(path.begin(), path.end(), '\\', '/');
}

InterpreterWrapper::InterpreterWrapper(PluginConfiguration& pluginConfig, std::string nameApp)
    : factory(nullptr)
    , cfg(pluginConfig)
{

    cfg.path.wwiseRoot = SysCall::getEnvVar("WWISEROOT");
    cfg.path.exportPath = cfg.path.wwiseRoot + '/' + std::string(STORAGE_DIRNAME);
    normalizePath(cfg.path.exportPath);
    if (!std::filesystem::exists(cfg.path.exportPath)) {
        std::filesystem::create_directory(cfg.path.exportPath);
    }

    cfg.name_app = nameApp;
    cfg.path.cppfile = "";

    cfg.path.faust_includedir = SysCall::execCommand("faust --includedir");
    cfg.path.faust_dspdir = SysCall::execCommand("faust --dspdir");

    normalizePath(cfg.path.faust_includedir);
    normalizePath(cfg.path.faust_dspdir);

    cfg.path.archfile = cfg.path.faust_dspdir + "/wwise.cpp";
    // TODO: store in a file maybe?
}

// void InterpreterWrapper::set_exportPath(const std::string& path)
// {
//     cfg.path.exportPath = path;
// }

InterpreterWrapper::~InterpreterWrapper()
{
    if (factory)
    {
        deleteInterpreterDSPFactory(factory);
        factory = nullptr;
    }
}

std::string InterpreterWrapper::get_default_entry_code()
{
    return default_entry_code;
}

bool InterpreterWrapper::compileDSP(const std::string& dspCode)
{

    // directly create a new dsp factory to exploit its caching ability 
    // const std::string &name_app, const std::string &dsp_content, int argc, const char *argv[], std::string &error_msg)
    // TODO: fix compilation options
    // Define compilation options (none for now, but could add e.g. "-vec")
    // int argc = 0;
    // const char** argv = nullptr;
    const char* argv[] = { 
        "-a", cfg.path.archfile.c_str(),
        "-I", cfg.path.faust_includedir.c_str(), 
        "-I", cfg.path.faust_dspdir.c_str(),
        "-lang", "cpp", 
        "-json"
    };
    int argc = 9;

    std::string errorMessage;
    interpreter_dsp_factory* newFactory = createInterpreterDSPFactoryFromString(
        cfg.name_app,
        dspCode,
        argc,
        argv,
        errorMessage
    );

    if (!newFactory) 
    {
        std::cerr << "Faust compilation failed : " << errorMessage << std::endl;
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
    return true;
}

bool InterpreterWrapper::exportCPP() {
    
    cfg.path.tempDir = createTempDir();
    cfg.path.cppfile = cfg.path.tempDir + '/' + cfg.name_app + ".cpp";

    if (!factory) {
        std::cerr << "No compiled DSP factory available for export." << std::endl;
        return false;
    }

    if (!writeInterpreterDSPFactoryToBitcodeFile(factory, cfg.path.cppfile)) {
        std::cerr << "Failed to write DSP bytecode to file: " << cfg.path.cppfile << std::endl;
        return false;
    }

    return true;
}

bool InterpreterWrapper::exportJSON()
{
    // move the json file 
    std::string json_dest = cfg.path.tempDir + '/' + cfg.name_app + ".json";
    bool jsonMoved = moveFile(cfg.name_app+".json", json_dest);
    if (!jsonMoved){
        cfg.path.json_file = cfg.name_app+".json"; // in the cwd
        return false;
    }
    cfg.path.json_file = json_dest;
    return true;
}

bool InterpreterWrapper::compileCPP()
{
    // @TODO integration dependent path --> will be installed in smth like : faust/architecture/wwise/interpreter/CMakeLists.txt
    std::filesystem::path currFile = __FILE__;  // CMakeLists.txt is located in the same dir
    std::string cmakelists_dir = currFile.parent_path().string(); // dir where the CMakeLists.txt is located
    std::string cmd = 
        "cmake -B \"" + cfg.path.tempDir + "\" "
        "-S \"" + cmakelists_dir + "\" "
        "-DFAUST_INCLUDE_DIR=\"" + cfg.path.faust_includedir + "\" "
        "-DFAUST_LIBRARY=\"" + cfg.path.faust_dspdir + "\" "
        "-DPROJECT_NAME=" + cfg.name_app + " "
        "-DFAUST_CPP_FILE=\"" + cfg.path.cppfile + "\" "
        "&& cmake --build \"" + cfg.path.tempDir + "\" --config "+ cfg.path.config;

    int result = std::system(cmd.c_str());
    if(result != 0)
    {
        std::cerr << "Cpp file could not be compiled."<<std::endl;
        std::cerr << "Build command failed with code " << result << std::endl;
        return false;
    }

    cfg.path.dllPath = cfg.path.tempDir + "/"+ cfg.path.config +"/" + cfg.name_app + ".dll";
    
    return true;
}

bool InterpreterWrapper::moveFile(const std::string& sourcePath, const std::string& destPath)
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

bool InterpreterWrapper::buildPlugin(const std::string& dspCode) {

    cfg.path.tempDir = createTempDir();

    // write dspCode into the example.dsp file. Name of plugin will be set automatically via declare.
    std::filesystem::path dspPath = std::filesystem::path(cfg.path.tempDir + "/example.dsp");
    std::ofstream outFile(dspPath);
    if (!outFile.is_open()) {
        return false;
    }
    outFile << dspCode;
    outFile.close();

    // call faust2wwise inside the temp directory
    std::ostringstream cmd;
    cmd << "cd /d \"" << cfg.path.tempDir << "\" && faust2wwise example.dsp > output.log 2>&1";

    int result = std::system(cmd.str().c_str());

    return result == 0;
}

// TODO: discard timestamp, and use the plugin name for the new directory.
std::string InterpreterWrapper::createTempDir()
{
    std::ostringstream timestamp;
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    timestamp << std::put_time(std::localtime(&now_time_t), "%Y%m%d_%H%M%S");

    std::filesystem::path tempDirectory = std::filesystem::path( cfg.path.exportPath + "/faust_" + timestamp.str());

    try {
        std::filesystem::create_directories(tempDirectory);
    } catch (const std::filesystem::filesystem_error& e) {
        // handle failure to create directory
        return false;
    }
    return tempDirectory.string();
}