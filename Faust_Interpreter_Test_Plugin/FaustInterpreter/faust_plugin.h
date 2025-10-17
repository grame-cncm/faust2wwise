#ifndef FAUST_PLUGIN_H
#define FAUST_PLUGIN_H

#include <atomic>

#include "dll_state.h"
#include "runtime_link.h"

#include <faust/dsp/dsp.h>
#include <faust/gui/MapUI.h>
#include <faust/gui/meta.h>

extern dsp* m_dsp_ptr;
extern MapUI* map_ui_ptr;
extern void (*initDSP_ptr)(int);
extern void (*setParameter_float_ptr)(const char*, float);
extern FAUSTFLOAT (*getParameter_ptr)(const char*);

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
    ~FaustPlugin();
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

    // Forward declarations as pointers (extern means they are defined elsewhere)
    dsp* m_dsp_ptr = nullptr;
    MapUI* map_ui_ptr = nullptr;

    void (*initDSP_ptr)(int) = nullptr;
    void (*setParameter_float_ptr)(const char*, float) = nullptr;
    FAUSTFLOAT (*getParameter_ptr)(const char*) = nullptr;
};

#endif