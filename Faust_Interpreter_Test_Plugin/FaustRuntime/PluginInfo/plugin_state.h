#ifndef PLUGIN_STATE_H
#define PLUGIN_STATE_H

enum PluginState{
    ZERO_STATE,
    // IDDLE, // for when loading previously compiled plugins? i.e. when opening wwise?
    PENDING_COMPILATION,
    ERR_COMPILE_DSP,
    ERR_EXPORT_CPP,
    ERR_EXPORT_JSON,
    ERR_COMPILE_CPP,
    DLL_COMPILED,
    ERR_LINKAGE,
    DLL_LINKED,
    ERR_LINKED,
    ERR_JSON_PARSE,
    JSON_PARSED,
    ERR_LOAD_SYMBOLS,
    SYMBOLS_LOADED,
    READY
};

#endif