#ifndef FAUST_PLUGIN_LOADER_H
#define FAUST_PLUGIN_LOADER_H


#include "PluginInfo/plugin_state.h"
#include "JsonProcessor/json_parser.h"
#include "PluginTemplate/SourcePlugin.h"
#include "PluginTemplate/EffectPlugin.h"

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

    void setPluginState(PluginState);
    PluginState getPluginState();
    
    void setupAudio(int, int*);

    bool initPlugin();
    void unloadPlugin();
    void callback(std::vector<FAUSTFLOAT*>&, const AkUInt32);
    
    ParameterList& getParameters();
    
private:
    /*! @brief Class constructor.
    */
    PluginLoader();
    bool loadDynamicLib();

    void* dspLib; // temporary pointer to library

    std::atomic<PluginState> pluginState;

    PluginConfiguration cfg;
    
    AbstractPlugin *plugin;
    // preallocate
    WwiseEffectPlugin effectPlugin;
    WwiseSourcePlugin sourcePlugin;

    ParameterList parameters;
};

#endif