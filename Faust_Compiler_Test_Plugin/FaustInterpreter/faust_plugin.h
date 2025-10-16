#ifndef FAUST_PLUGIN_H
#define FAUST_PLUGIN_H

#include <atomic>

#include "dll_state.h"
#include "runtime_link.h"

/*!
 * @brief Singleton class.
*/
class FaustPlugin{

public:
    
    /*! @brief Get instance of the FaustPlugin class. Singleton pattern.
    * @return FaustPlugin& - a reference to the FaustPlugin object.
    */
    static FaustPlugin& getInstance(){
        static FaustPlugin faust_plugin;
        return faust_plugin;
    }
    FaustPlugin(const FaustPlugin&) = delete; // delete copy constructor
    FaustPlugin& operator=(const FaustPlugin&) = delete; // delete copy assignment operator
    FaustPlugin(FaustPlugin&&) = delete; // delete move constructor
    FaustPlugin& operator=(FaustPlugin&&) = delete; //  delete move assignement operator


    void setPluginState(DLLState);
    DLLState getPluginState();
    void loadDynamicLib(const std::string&);
    
private:
    /*! @brief Class constructor.
    */
    FaustPlugin();
    
    void* dspLib; // temporary pointer to library
    
    std::atomic<DLLState> dllState;

};

#endif