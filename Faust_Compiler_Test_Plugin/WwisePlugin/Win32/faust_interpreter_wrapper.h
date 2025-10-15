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

    // these two should be united.. This is the preview function. 
    // Compiles a cpp file and integrates it directly into a dynamic plugin.
    bool compile(const std::string& dspCode);
    
    bool buildPlugin(const std::string& dspCode);
private:

    std::string name_app;

    interpreter_dsp_factory* factory;
    std::string lastDSPCode, currentSHA;

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

    //filePath -> should be a temporary dir for storing the dynamic plugin.
    bool exportCPP();
    void compileCPP_async();
    std::filesystem::path createTempDir();
    bool moveFile(const std::string&, const std::string&);
};

#endif