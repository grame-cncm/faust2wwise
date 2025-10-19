#include "abstract.h"

AbstractPlugin::~AbstractPlugin(){
    reset();
}

void AbstractPlugin::setup(PluginConfiguration &config)
{
    cfg = &config;

    faust_outputs.resize(cfg->num_outputs);

    initDSP_(cfg->sampleRate);

}

bool AbstractPlugin::loadSymbols(void* dspLib)
{
    // load symbols
    initDSP_ = reinterpret_cast<void (*)(int)>(RuntimeLink::getSymbol(dspLib, "initDSP"));
    setParameter_float_ = reinterpret_cast<void (*)(const char*, float)>(RuntimeLink::getSymbol(dspLib,"setParameter"));
    getParameter_ = reinterpret_cast<FAUSTFLOAT (*)(const char*)>(RuntimeLink::getSymbol(dspLib, "getParameter"));
    getDSP_ = reinterpret_cast<dsp*(*)()>(RuntimeLink::getSymbol(dspLib, "getDSP"));
    getMapUI_ = reinterpret_cast<MapUI*(*)()>(RuntimeLink::getSymbol(dspLib, "getMapUI"));
    m_dsp_ = getDSP_();
    map_ui_ = getMapUI_();
    
    if (!initDSP_ || !setParameter_float_ || !getParameter_ || !getDSP_ || !getMapUI_)
        return false;
    return true;
}

void AbstractPlugin::reset()
{
    initDSP_ = nullptr;
    setParameter_float_ = nullptr;
    getParameter_ = nullptr;
    getDSP_ = nullptr;
    getMapUI_ = nullptr;
    m_dsp_ = nullptr;
    map_ui_ = nullptr;
    cfg = nullptr;
}