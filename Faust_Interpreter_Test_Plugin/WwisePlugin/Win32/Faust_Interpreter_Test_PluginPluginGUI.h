/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/
#pragma once

#include "../Faust_Interpreter_Test_PluginPlugin.h"
#include <string>
#include "faust_interpreter_wrapper.h"
#include "faust_plugin.h"

enum WM_STATE{
    INIT_STATE=1,
    RUN_STATE=2,
    EXIT_STATE=3,
    NONINIT_STATE=0
};

class Faust_Interpreter_Test_PluginPluginGUI final
	: public AK::Wwise::Plugin::PluginMFCWindows<>
	, public AK::Wwise::Plugin::GUIWindows
{
public:
	Faust_Interpreter_Test_PluginPluginGUI();

	HINSTANCE GetResourceHandle() const override;

    bool GetDialog(
        AK::Wwise::Plugin::eDialog in_eDialog,
        UINT& out_uiDialogID,
        AK::Wwise::Plugin::PopulateTableItem*& out_pTable
    ) const override;

    bool WindowProc(
        AK::Wwise::Plugin::eDialog in_eDialog,
        HWND in_hWnd,
        uint32_t in_message,
        WPARAM in_wParam,
        LPARAM in_lParam,
        LRESULT& out_lResult) override;

private:
    std::wstring dspCode;
    HWND faustWnd,editorWnd;
    
    WM_STATE state;
    
    bool SetCodeEditorText(); 
    bool SaveCodeEditorText();
    bool OnPreviewButtonClicked();
    bool OnBuildButtonClicked();
    void ShowPopupWindow();

    bool loadLastSavedCode();
    bool saveCurrentCodeState();

    void debugPrint(std::wstring, size_t);

    FaustInterpreterWrapper faustInterpreter;
    std::wstring entry_code;

    const char* wwiseRoot;
    std::filesystem::path tempDir;

    FaustPlugin& faust_plugin = FaustPlugin::getInstance();

};
