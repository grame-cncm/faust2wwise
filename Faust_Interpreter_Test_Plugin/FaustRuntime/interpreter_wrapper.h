#ifndef FAUST_INTERPRETER_WRAPPER_H
#define FAUST_INTERPRETER_WRAPPER_H

#include "faust/dsp/interpreter-dsp.h"
#include "JsonProcessor/json_parser.h"
#include "PluginInfo/plugin_config.h"
#include <string>
#include <filesystem>

class InterpreterWrapper {

public:
    InterpreterWrapper(PluginConfiguration&); // receives the temp dir to export stuff..
    ~InterpreterWrapper();

    std::string getExportPath( ) { return cfg.path.exportPath; }

    std::string get_default_entry_code();

    void setPluginConfiguration(PluginConfiguration& cfg);

    // Preview related functions..
    bool compileDSP(const std::string& dspCode); // Compiles a cpp file and recompiles it directly into a dynamic library.
    bool exportCPP();  // moves the cpp file into the appropriate export dir.
    bool exportJSON(); // moves the json file into the appropriate export dir.
    bool compileCPP(); // compiles the cpp file into a dynamic library.

    bool buildPlugin(const std::string& dspCode);

private:

    interpreter_dsp_factory* factory;
    std::string lastDSPCode, currentSHA;

    PluginConfiguration &cfg;

    const std::string default_entry_code = R"FAUST(
import("stdfaust.lib");

declare name "myPlugin"; // declare name of your plugin

// you faust dsp code here..
)FAUST";

    bool moveFile(const std::string&, const std::string&);
    std::string createTempDir();
};

#endif