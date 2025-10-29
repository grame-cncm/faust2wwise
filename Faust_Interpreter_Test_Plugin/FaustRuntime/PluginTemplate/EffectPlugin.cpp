#include "EffectPlugin.h"

WwiseEffectPlugin::WwiseEffectPlugin(PluginConfiguration& config, ParameterList& params, InterpreterWrapper& interpreter)
    : AbstractPlugin(config, params, interpreter)
{

}

WwiseEffectPlugin::~WwiseEffectPlugin()
{

}

bool WwiseEffectPlugin::setup()
{
    return AbstractPlugin::setup();
    // ...
}

void WwiseEffectPlugin::callback(std::vector<FAUSTFLOAT*>& outputs, const AkUInt32 bufferSize)
{

}

void WwiseEffectPlugin::fillRestOfBuffersWithSilence(const AkUInt32 channels)
{
    
}