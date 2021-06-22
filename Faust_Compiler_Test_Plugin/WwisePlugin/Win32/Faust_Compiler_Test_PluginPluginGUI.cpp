
#include "Faust_Compiler_Test_PluginPluginGUI.h"

Faust_Compiler_Test_PluginPluginGUI::Faust_Compiler_Test_PluginPluginGUI()
{
}

ADD_AUDIOPLUGIN_CLASS_TO_CONTAINER(
    Faust_Compiler_Test_Plugin,            // Name of the plug-in container for this shared library
    Faust_Compiler_Test_PluginPluginGUI,   // Authoring plug-in class to add to the plug-in container
    Faust_Compiler_Test_PluginFX           // Corresponding Sound Engine plug-in class
);
