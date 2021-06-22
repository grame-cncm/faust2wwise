#pragma once

#include "../Faust_Compiler_Test_PluginPlugin.h"

class Faust_Compiler_Test_PluginPluginGUI final
	: public AK::Wwise::Plugin::PluginMFCWindows<>
	, public AK::Wwise::Plugin::GUIWindows
{
public:
	Faust_Compiler_Test_PluginPluginGUI();

};
