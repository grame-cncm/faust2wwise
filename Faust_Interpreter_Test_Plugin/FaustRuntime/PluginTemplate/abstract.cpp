#include "abstract.h"

AbstractPlugin::AbstractPlugin(PluginConfiguration& config, ParameterList& params, InterpreterWrapper& interpreter)
    : cfg(config)
    , parameters(params)
    , faustInterpreter(interpreter)
{
}

AbstractPlugin::~AbstractPlugin(){
    reset();
}

bool AbstractPlugin::setup()
{
    faust_outputs.resize(cfg.num_outputs);

    // this function always return true. 
    // But it is a (not pure) virtual, so that means:
    // children are allowed to return false based on their extended implementation.
    // @TODO : if childeren have no reason to return false, change signature to void return.
    return true;
}

void AbstractPlugin::reset()
{
    faust_outputs.clear();

}