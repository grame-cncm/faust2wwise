#include "SourcePlugin.h"

WwiseSourcePlugin::WwiseSourcePlugin(PluginConfiguration& config, ParameterList& params, InterpreterWrapper& interpreter)
    : AbstractPlugin(config, params, interpreter)
{

}

bool WwiseSourcePlugin::setup()
{
    return AbstractPlugin::setup();
    // ...
}

void WwiseSourcePlugin::callback(std::vector<FAUSTFLOAT*>& outputs, const AkUInt32 bufferSize)
{
    // set parameters
    for (auto& p : parameters){
        if (p.type!="bargraph")
        {
            faustInterpreter.setParameter(p.shortname,p.value.load());
        }
        else
        {
            p.value.store(faustInterpreter.getParameter(p.shortname));
        }
    }

    for (AkUInt32 ch = 0; ch < cfg.num_outputs; ++ch)
    {
        faust_outputs[ch] = outputs[ch];
    }
        
    faustInterpreter.callback(bufferSize, nullptr, faust_outputs.data());


}