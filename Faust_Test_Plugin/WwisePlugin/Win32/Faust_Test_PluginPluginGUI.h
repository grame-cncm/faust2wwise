#pragma once

#include "../Faust_Test_PluginPlugin.h"

class Faust_Test_PluginPluginGUI final
	: public AK::Wwise::Plugin::PluginMFCWindows<>
	, public AK::Wwise::Plugin::GUIWindows
{
public:
	Faust_Test_PluginPluginGUI();

};
