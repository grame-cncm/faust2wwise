#ifndef FAUST_INTERPRETER_WRAPPER_H
#define FAUST_INTERPRETER_WRAPPER_H

#include <string>
#include <filesystem>
#include "faust/dsp/interpreter-dsp.h"

class FaustInterpreterWrapper {

public:
    FaustInterpreterWrapper(std::string nameApp="MY_DSP"); // receives the temp dir to export stuff..
    ~FaustInterpreterWrapper();

    void set_exportPath(const std::string& path);

    std::string get_default_entry_code();

    // Compiles a cpp file and recompiles it directly into a dynamic library.
    bool previewPlugin(const std::string& dspCode);
    
    bool buildPlugin(const std::string& dspCode);

    std::string get_dll_path()
    {
        return tempDir.string() + "/Release/" + name_app + ".dll"; 
    }


private:

    interpreter_dsp_factory* factory;
    std::string lastDSPCode, currentSHA;

    std::string name_app;
    std::string faust_includedir;
    std::string faust_dspdir;
    std::string exportPath;
    std::filesystem::path tempDir;
    std::string cppfile;
    std::string archfile;
    // @TODO Wrap those paths

    const std::string default_entry_code = R"FAUST(
import("stdfaust.lib");

declare name "myPlugin"; // declare name of your plugin

// you faust dsp code here..
)FAUST";

    bool exportCPP();
    bool compileCPP();
    bool moveFile(const std::string&, const std::string&);
    std::filesystem::path createTempDir();
};

#endif