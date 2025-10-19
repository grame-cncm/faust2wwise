#include "EffectPlugin.h"

WwiseEffectPlugin::~WwiseEffectPlugin()
{

}

void WwiseEffectPlugin::setup(PluginConfiguration &config)
{
    AbstractPlugin::setup(config);

}

void WwiseEffectPlugin::callback(std::vector<FAUSTFLOAT*>& outputs, const AkUInt32 bufferSize)
{

}

void WwiseEffectPlugin::fillRestOfBuffersWithSilence(const AkUInt32 channels)
{
    
}