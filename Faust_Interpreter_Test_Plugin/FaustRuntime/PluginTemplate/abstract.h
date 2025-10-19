#ifndef TEMPLATE_PLUGIN_H
#define TEMPLATE_PLUGIN_H

#include "PluginInfo/plugin_config.h"
#include "PluginUtils/runtime_link.h"
#include <faust/dsp/dsp.h>
#include <faust/gui/MapUI.h>
#include <faust/gui/meta.h>
#include <vector>

typedef uint32_t AkUInt32;

class AbstractPlugin{
public:

    virtual ~AbstractPlugin();
    
    bool loadSymbols(void* dspLib);
    
    virtual void setup(PluginConfiguration &config);
    
    virtual void callback(std::vector<FAUSTFLOAT*>&, const AkUInt32)=0;
    
    virtual void fillRestOfBuffersWithSilence(const AkUInt32)=0;

    void reset();
    
protected:

    PluginConfiguration* cfg;

    dsp* m_dsp_ = nullptr;
    MapUI* map_ui_ = nullptr;
    void (*initDSP_)(int) = nullptr;
    void (*setParameter_float_)(const char*, float) = nullptr;
    FAUSTFLOAT (*getParameter_)(const char*) = nullptr;

    std::vector<FAUSTFLOAT*> faust_outputs;

private:

    dsp* (*getDSP_)() = nullptr;
    MapUI* (*getMapUI_)() = nullptr;
};

#endif