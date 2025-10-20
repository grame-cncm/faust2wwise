#include "SourcePlugin.h"

WwiseSourcePlugin::WwiseSourcePlugin(std::vector<Parameter> &params)
    : AbstractPlugin(params)
{

}

// WwiseSourcePlugin::~WwiseSourcePlugin()
// {

// }

void WwiseSourcePlugin::setup(PluginConfiguration &config)
{
    AbstractPlugin::setup(config);
    // ..
}

void WwiseSourcePlugin::callback(std::vector<FAUSTFLOAT*>& outputs, const AkUInt32 bufferSize)
{
    // <<FOREACHPARAM:IF io_type==input: setParameter( "${shortname}", m_pParams->${isRTPC}.${RTPCname} );>>

    for (AkUInt32 ch = 0; ch < cfg->num_outputs; ++ch)
    {
        faust_outputs[ch] = outputs[ch];
    }

    // fillRestOfBuffersWithSilence(bufferSize);
        
    m_dsp_->compute(bufferSize, nullptr, faust_outputs.data());


}

void WwiseSourcePlugin::fillRestOfBuffersWithSilence(const AkUInt32 bufferSize)
{
    
    // Runs only once filling the rest of the faust output channels with silence,  ...
    // ... and only in case the channels requested by the faust dsp program is greater 
    // than the available channels wwise can support.

    // static std::vector<FAUSTFLOAT> silenceBuffer;
    // if (silenceBuffer.size() < bufferSize) {
    //     silenceBuffer.resize(bufferSize, 0.0f);

    // }
    
    // // Fill silent channels
    // if (!faust_outputs[*cfg->speakersAvail])
    // {
    //     for (AkUInt32 ch = (*cfg->speakersAvail); ch < static_cast<AkUInt32>(cfg->num_outputs); ++ch) {
    //         faust_outputs[ch] = silenceBuffer.data();
    //     }
        // *cfg->speakersAvail = cfg->num_outputs;
    // }
    // *cfg->speakersAvail = cfg->num_outputs;

}
