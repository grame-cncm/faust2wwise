#ifndef FAUST_PLUGIN_LOADER_H
#define FAUST_PLUGIN_LOADER_H

#include "PluginUtils/syscall.h"
// #include "Interpreter/debugDSP.h"
#include "Interpreter/wrapper.h"
#include "PluginTemplate/SourcePlugin.h"
#include "PluginTemplate/EffectPlugin.h"
#include "PluginInfo/plugin_state.h"

/*!
 * @brief Singleton class.
*/
class PluginLoader{

public:
    
    /*! @brief Get instance of the PluginLoader class. Singleton pattern.
    * @return PluginLoader& - a reference to the PluginLoader object.
    */
    static PluginLoader& getInstance(){
        static PluginLoader faust_plugin;
        return faust_plugin;
    }
    ~PluginLoader();
    PluginLoader(const PluginLoader&) = delete; // delete copy constructor
    PluginLoader& operator=(const PluginLoader&) = delete; // delete copy assignment operator
    PluginLoader(PluginLoader&&) = delete; // delete move constructor
    PluginLoader& operator=(PluginLoader&&) = delete; //  delete move assignement operator

    PluginConfiguration& getConfiguration() {   return cfg;     }
    ParameterList& getParameters()          {   return parameters;  }
    PluginState getPluginState()            {   return pluginState.load();  }

    bool createPlugin(std::string&); // TODO copy string?
    int setupAudio(int);
    void unloadPlugin();
    void callback(std::vector<FAUSTFLOAT*>&, const AkUInt32);
    
    bool buildPlugin(const std::string&);
    std::string& getExportPath() {   return cfg.path.exportPath;  }

private:
    /*! @brief Class constructor.
    */
    PluginLoader();

    InterpreterWrapper faustInterpreter;

    std::atomic<PluginState> pluginState;
    PluginConfiguration cfg;
    ParameterList parameters;
    
    AbstractPlugin *plugin;
    // preallocate
    WwiseEffectPlugin effectPlugin;
    WwiseSourcePlugin sourcePlugin;

};

#endif