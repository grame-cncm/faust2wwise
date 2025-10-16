#include "faust_plugin.h"

FaustPlugin::FaustPlugin()
    :dllState(DLLState::UNSET)
{

}

void FaustPlugin::setPluginState(DLLState state)
{
    dllState.store(state);
}

DLLState FaustPlugin::getPluginState()
{
    return dllState.load();
}