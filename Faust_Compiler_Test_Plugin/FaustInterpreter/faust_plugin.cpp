#include "faust_plugin.h"

FaustPlugin::FaustPlugin()
    :dllState(0)
{

}

void FaustPlugin::setPluginState(int state)
{
    dllState.store(state);
}

int FaustPlugin::getPluginState()
{
    return dllState.load();
}