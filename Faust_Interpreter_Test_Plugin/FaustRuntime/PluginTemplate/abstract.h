#ifndef TEMPLATE_ABSTRACT_PLUGIN_H
#define TEMPLATE_ABSTRACT_PLUGIN_H

#include "PluginInfo/plugin_config.h"
#include "Parameters/parameter.h"
// #include "Interpreter/debugDSP.h"
#include "Interpreter/wrapper.h"

typedef uint32_t AkUInt32;

class AbstractPlugin{
public:

    AbstractPlugin(PluginConfiguration&, ParameterList&, InterpreterWrapper&);

    virtual ~AbstractPlugin();
    
    virtual void reset();

    virtual void setup();    

    virtual void callback(std::vector<FAUSTFLOAT*>&, const AkUInt32)=0;
    
protected:

    void setParameters();

    PluginConfiguration& cfg;
    ParameterList& parameters;
    InterpreterWrapper& faustInterpreter;

    std::vector<FAUSTFLOAT*> faust_outputs;

};

#endif