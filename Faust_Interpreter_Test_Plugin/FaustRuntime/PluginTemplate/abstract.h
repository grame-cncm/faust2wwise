#ifndef TEMPLATE_PLUGIN_H
#define TEMPLATE_PLUGIN_H

#include "PluginInfo/plugin_config.h"
#include "Parameters/parameter.h"
// #include "Interpreter/debugDSP.h"
#include "Interpreter/wrapper.h"

typedef uint32_t AkUInt32;

class AbstractPlugin{
public:

    AbstractPlugin();

    AbstractPlugin(PluginConfiguration&, ParameterList&, InterpreterWrapper&);

    virtual ~AbstractPlugin();
    
    void reset();

    virtual bool setup();    
    virtual void callback(std::vector<FAUSTFLOAT*>&, const AkUInt32)=0;
    
protected:

    PluginConfiguration& cfg;
    ParameterList& parameters;
    InterpreterWrapper& faustInterpreter;

    std::vector<FAUSTFLOAT*> faust_outputs;

};

#endif