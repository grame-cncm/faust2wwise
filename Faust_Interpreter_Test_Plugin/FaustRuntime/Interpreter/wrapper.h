#ifndef FAUST_INTERPRETER_WRAPPER_H
#define FAUST_INTERPRETER_WRAPPER_H

#include "faust/dsp/interpreter-dsp.h"
#include "PluginInfo/plugin_config.h"
#include "PluginUtils/plugin_utils.h"
#include "Parameters/myMapUI.h"
#include "Parameters/parameter.h"
#include <faust/dsp/dsp.h>
#include <string>
#include <filesystem>

class InterpreterWrapper {

public:
    InterpreterWrapper();
    ~InterpreterWrapper();

    void reset();

    bool compileDSP(const std::string dspCode, const PluginConfiguration& cfg);
    
    bool createDSPandUI();

    bool configurePlugin(PluginConfiguration&, ParameterList&);

    void setupDSP(int);

    void callback(int, FAUSTFLOAT**, FAUSTFLOAT**);

    template <typename T>
    void setParameter(const std::string name, T value)
    {
        mapui->setParamValue(name, static_cast<FAUSTFLOAT>(value)); 
    }

    FAUSTFLOAT getParameter(const std::string& name);

    bool exportCPP(const std::string& faust_dspdir, const std::string &filename, const std::string& dspCode, const std::string& filepath, bool doublePrecision, std::string& errorMessage);

private:

    interpreter_dsp_factory* factory;
    dsp* mdsp;
    myMapUI* mapui;
    std::string currentSHA;
    
    void setPluginConfiguration(PluginConfiguration&);
    void setParameterList(ParameterList&, PluginConfiguration&); 
};

#endif