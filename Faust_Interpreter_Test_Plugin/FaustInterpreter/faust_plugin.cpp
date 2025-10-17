#include "faust_plugin.h"

FaustPlugin::FaustPlugin()
    : dllState(DLLState::UNSET)
    , dspLib(nullptr)
{

}

FaustPlugin::~FaustPlugin()
{
    if (dspLib != nullptr)
        RuntimeLink::unloadDynamicLibrary(dspLib);
}

void FaustPlugin::loadDynamicLib(const std::string& libPath)
{
    if (dspLib != nullptr)
        RuntimeLink::unloadDynamicLibrary(dspLib);

    dspLib = RuntimeLink::loadDynamicLibrary(libPath);
    if (!dspLib)
    {
        dllState.store(FAIL_LINKAGE);
        return;
    }
    dllState.store(LINKED);
    m_dsp_ptr = reinterpret_cast<dsp*>(RuntimeLink::getSymbol(dspLib,"m_dsp"));
    map_ui_ptr = reinterpret_cast<MapUI*>(RuntimeLink::getSymbol(dspLib,"map_ui"));
    initDSP_ptr = reinterpret_cast<void (*)(int)>(RuntimeLink::getSymbol(dspLib, "initDSP"));
    setParameter_float_ptr = reinterpret_cast<void (*)(const char*, float)>(RuntimeLink::getSymbol(dspLib,"setParameter"));
    getParameter_ptr = reinterpret_cast<FAUSTFLOAT (*)(const char*)>(RuntimeLink::getSymbol(dspLib, "getParameter"));
}

void FaustPlugin::setPluginState(DLLState state)
{
    dllState.store(state);
}

DLLState FaustPlugin::getPluginState()
{
    return dllState.load();
}