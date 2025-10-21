#include "plugin_loader.h"

PluginLoader::PluginLoader()
    : pluginState(PluginState::ZERO_STATE)
    , dspLib(nullptr)
    , effectPlugin(parameters)
    , sourcePlugin(parameters)
{

}

PluginLoader::~PluginLoader()
{
    unloadPlugin();
}

void PluginLoader::setupAudio(int SR, int* channelsRequested)
{
    cfg.sampleRate = SR;
    *channelsRequested = cfg.num_outputs;

    plugin->setup(cfg);
    pluginState.store(PluginState::READY);
}

bool PluginLoader::loadDynamicLib()
{
    if (dspLib)
    {
        RuntimeLink::unloadDynamicLibrary(dspLib);
        dspLib = nullptr;
    }    

    dspLib = RuntimeLink::loadDynamicLibrary(cfg.path.dllPath);

    if (!dspLib)
        return false;
    return true;
}

bool PluginLoader::initPlugin()
{
    // load 
    bool dllLinked = loadDynamicLib();
    if (!dllLinked)
    {
        pluginState.store(PluginState::ERR_LINKAGE);
        return false;
    }
    pluginState.store(PluginState::DLL_LINKED);

    // load json
    bool jsonParsed = process_json_configuration(cfg, parameters);
    if (!jsonParsed)
    {
        pluginState.store(PluginState::ERR_JSON_PARSE);
        return false;
    }
    pluginState.store(PluginState::JSON_PARSED);

    // set plugin
    if (cfg.plugin_type == "source")
    {
        plugin = &sourcePlugin;
    }
    else if (cfg.plugin_type == "effect")
    {
        plugin = &effectPlugin;
    }

    // load symbols
    bool symbolsLoaded = plugin->loadSymbols(dspLib);
    if (!symbolsLoaded)
    {
        pluginState.store(PluginState::ERR_LOAD_SYMBOLS);
        return false;
    }
    pluginState.store(PluginState::SYMBOLS_LOADED);  
    
    // setupAudio is pending..
}

void PluginLoader::unloadPlugin()
{
    //NOTE: For now, apart from the constructor, this function is never yet called..

    PluginState state = pluginState.load();

    // first ensure playback is paused within Execute function
    if (state == PluginState::READY)
    {
        pluginState.store(PluginState::ZERO_STATE);
    }

    // reset plugin
    if (plugin)
       plugin->reset();

    // unload plugin
    plugin = nullptr;

    // reset config
    cfg.reset();

    // unload library
    if (dspLib){
        RuntimeLink::unloadDynamicLibrary(dspLib);
        dspLib = nullptr;
    }
        
}

void PluginLoader::setPluginState(PluginState state)
{
    pluginState.store(state);
}

PluginState PluginLoader::getPluginState()
{
    return pluginState.load();
}

void PluginLoader::callback(std::vector<FAUSTFLOAT*>& data, const AkUInt32 size)
{
    plugin->callback(data, size);
}


ParameterList& PluginLoader::getParameters()
{    
    return parameters;  
}
