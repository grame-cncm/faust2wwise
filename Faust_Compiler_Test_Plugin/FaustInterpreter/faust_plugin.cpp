#include "faust_plugin.h"

FaustPlugin::FaustPlugin()
    : dllState(DLLState::UNSET)
    , dspLib(nullptr)
{

}

void FaustPlugin::loadDynamicLib(const std::string& libPath)
{
    dspLib = RuntimeLink::loadDynamicLibrary(libPath);
    if (!dspLib)
    {
        dllState.store(FAIL_LINKAGE);
        return;
    }
    dllState.store(LINKED);
}

void FaustPlugin::setPluginState(DLLState state)
{
    dllState.store(state);
}

DLLState FaustPlugin::getPluginState()
{
    return dllState.load();
}