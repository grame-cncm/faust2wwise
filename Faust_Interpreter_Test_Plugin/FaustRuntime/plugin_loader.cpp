#include "plugin_loader.h"

#include <iostream>

PluginLoader::PluginLoader()
    : pluginState(PluginState::ZERO_STATE)
    , effectPlugin(cfg, parameters, faustInterpreter)
    , sourcePlugin(cfg, parameters, faustInterpreter)
{
    cfg.path.wwiseRoot = SysCall::getEnvVar("WWISEROOT");
    cfg.path.exportPath = cfg.path.wwiseRoot + '/' + std::string(STORAGE_DIRNAME);

    if (!std::filesystem::exists(cfg.path.exportPath)) {
        std::filesystem::create_directory(cfg.path.exportPath);
    }

    cfg.path.faust_includedir = SysCall::execCommand("faust --includedir");
    cfg.path.faust_dspdir = SysCall::execCommand("faust --dspdir");

    cfg.path.archfile = cfg.path.faust_dspdir + "/wwise.cpp";

}

PluginLoader::~PluginLoader()
{
    unloadPlugin();
}

bool PluginLoader::createPlugin(std::string &dspCode)
{
    
    // reset everything before proceeding 
    unloadPlugin();

    pluginState.store(PluginState::PENDING_CREATION);
    
    bool dspCompiled = faustInterpreter.compileDSP(dspCode, cfg);
    if (!dspCompiled)
    {
        pluginState.store(PluginState::ERR_COMPILE_DSP);
        return false;
    }
    pluginState.store(PluginState::DSP_COMPILED);

    bool dspCreated = faustInterpreter.createDSP();
    if (!dspCreated)
    {
        pluginState.store(PluginState::ERR_DSP_CREATION);
        return false;
    }
    pluginState.store(PluginState::DSP_CREATED);

    bool pluginConfigured = faustInterpreter.configurePlugin(cfg, parameters);
    if (!pluginConfigured)
    {
        pluginState.store(PluginState::ERR_CONFIGURE_PLUGIN);
        return false;
    }
    pluginState.store(PluginState::PLUGIN_CONFIGURED);

    // assign plugin pointer to preallocated plugin depending on its type.
    if (cfg.plugin_type == "source")
    {
        plugin = &sourcePlugin;
    }
    else if (cfg.plugin_type == "effect")
    {
        plugin = &effectPlugin;
    }
    pluginState.store(PluginState::DSP_CREATED);
    
    bool isSetUp = plugin->setup();
    if (!isSetUp)
    {
        pluginState.store(PluginState::ERR_SETUP_PLUGIN);
        return false;
    }
    pluginState.store(PluginState::SETUP_PLUGIN_OK);
        

    // setupAudio is pending - will be called when the play button is pressed ...


    return true;
}

int PluginLoader::setupAudio(int SR)
{
    if (pluginState.load() == PluginState::SETUP_PLUGIN_OK)
    {
        faustInterpreter.setupDSP(SR);
        cfg.sampleRate = SR;
        
        pluginState.store(PluginState::READY);

        return cfg.num_outputs; // return channels requested 

    }
    return 0;
}

void PluginLoader::unloadPlugin()
{
    //NOTE: For now, apart from the destructor, this function is never yet called..

    // PluginState state = pluginState.load();

    // first ensure playback is paused within Execute function
    // if (state == PluginState::READY)
    // {
        pluginState.store(PluginState::ZERO_STATE);
    // }

    // reset plugin
    if (plugin)
       plugin->reset();
    
    // unload plugin
    plugin = nullptr;

    // reset internal
    cfg.reset();
    faustInterpreter.reset();

    // reset params
    parameters.clear();
    parameters.shrink_to_fit();
}

void PluginLoader::callback(std::vector<FAUSTFLOAT*>& outdata, const AkUInt32 size)
{
    plugin->callback(outdata, size);
}


// ParameterList& PluginLoader::getParameters()
// {    
//     return parameters;  
// }

#include <fstream>
#include <cstdlib>
#include <sstream>
bool PluginLoader::buildPlugin(const std::string& dspCode) {

    std::string tempDir = PluginUtils::createTempDir(cfg.path.exportPath);

    // write dspCode into the example.dsp file. Name of plugin will be set automatically via declare.
    std::filesystem::path dspPath = std::filesystem::path(tempDir + "/example.dsp");
    std::ofstream outFile(dspPath);
    if (!outFile.is_open()) {
        return false;
    }
    outFile << dspCode;
    outFile.close();

    // call faust2wwise inside the temp directory
    std::ostringstream cmd;
    cmd << "cd /d \"" << tempDir << "\" && faust2wwise example.dsp > output.log 2>&1";

    int result = std::system(cmd.str().c_str());

    return result == 0;
}
