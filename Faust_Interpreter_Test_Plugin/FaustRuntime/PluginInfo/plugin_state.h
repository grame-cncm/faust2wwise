#ifndef PLUGIN_STATE_H
#define PLUGIN_STATE_H

enum PluginState{
    ZERO_STATE,
    // IDDLE, // for when loading previously compiled plugins? i.e. when opening wwise?
    PENDING_CREATION,
    ERR_COMPILE_DSP,
    DSP_COMPILED,
    ERR_DSP_CREATION,
    DSP_CREATED,
    PENDING_CONFIGURATION,
    ERR_CONFIGURE_PLUGIN,
    PLUGIN_CONFIGURED,
    ERR_SETUP_PLUGIN,
    SETUP_PLUGIN_OK,
    BUILT,
    READY
};

#endif