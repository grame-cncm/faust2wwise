#include "faust_interpreter_wrapper.h"
#include <faust/dsp/libfaust.h> 
#include "utils.h"
#include <iostream>

FaustInterpreterWrapper::FaustInterpreterWrapper(std::string nameApp)
    : factory(nullptr)
    , name_app(nameApp)
{
    faust_includedir = execCommand("faust --includedir");
    faust_dspdir = execCommand("faust --dspdir");
    // TODO: store in a file maybe?
}

void FaustInterpreterWrapper::set_exportPath(const std::string& path)
{
    exportPath = path;
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

bool FaustInterpreterWrapper::compile(const std::string& dspCode)
{
    // directly create a new dsp factory to exploit its caching ability 
    // const std::string &name_app, const std::string &dsp_content, int argc, const char *argv[], std::string &error_msg)
    // TODO: fix compilation options
    // Define compilation options (none for now, but could add e.g. "-vec")
    // int argc = 0;
    // const char** argv = nullptr;
    const char* argv[] = { 
        "-I", faust_includedir.c_str(), 
        "-I", faust_dspdir.c_str(),
        "-lang", "cpp", 
        "-json"
    };
    int argc = 7;

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
    else std::cerr <<" Faust compilation success!"<<std::endl;

    if(factory && factory != newFactory){
        deleteInterpreterDSPFactory(factory);
    }

    //  update states
    factory = newFactory;
    lastDSPCode = dspCode;
    currentSHA = factory->getSHAKey();
    // name_app = factory->getName(); --> @TODO fix bug : Exception thrown at 0x00007FFA23425369 in Wwise.exe: Microsoft C++ exception: faustexception at memory location 0x000000C2BFAFE9C0. Unhandled exception at 0x00007FFA23425369 (KernelBase.dll) in Wwise.exe: 0xC000041D: An unhandled exception was encountered during a user callback.
    
    exportCPP();

    return true;

    // if (factory && dspCode == lastDSPCode){
    //     return true; // already compiled
    // }

    // // if new code is used
    // // TODO: Use a tool to check if needs new cache?
    // if (factory)
    // {
    //     deleteInterpreterDSPFactory(factory);
    //     factory = nullptr;
    // }
}

bool FaustInterpreterWrapper::exportCPP() {
    
    auto tempDir = createTempDir();
    std::string filePath = tempDir.string() + '/' + name_app + ".cpp";

    if (!factory) {
        std::cerr << "No compiled DSP factory available for export." << std::endl;
        return false;
    }

    if (!writeInterpreterDSPFactoryToBitcodeFile(factory, filePath)) {
        std::cerr << "Failed to write DSP bytecode to file: " << filePath << std::endl;
        return false;
    }

    // move the json file 
    moveFile(name_app+".json", tempDir.string() + '/' + name_app + ".json");

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

std::filesystem::path FaustInterpreterWrapper::createTempDir()
{
    std::ostringstream timestamp;
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    timestamp << std::put_time(std::localtime(&now_time_t), "%Y%m%d_%H%M%S");

    std::filesystem::path tempDir = std::filesystem::path(exportPath) / ("faust_" + timestamp.str());

    try {
        std::filesystem::create_directories(tempDir);
    } catch (const std::filesystem::filesystem_error& e) {
        // handle failure to create directory
        return false;
    }
    return tempDir;
}