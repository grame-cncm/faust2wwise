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

#include "Faust_Interpreter_Test_PluginPluginGUI.h"
#include "../resource.h"
#include "PluginUtils/syscall.h"
#include "PluginUtils/plugin_utils.h"
#include "AudioInput/audio_inputs.h"
#include <AK/AkWwiseSDKVersion.h>
#include "entry_code.h"
#include <windows.h>
#include <sstream>
#include <thread>

AK_WWISE_PLUGIN_GUI_WINDOWS_BEGIN_POPULATE_TABLE(PropertyTable)
AK_WWISE_PLUGIN_GUI_WINDOWS_END_POPULATE_TABLE()

Faust_Interpreter_Test_PluginPluginGUI::Faust_Interpreter_Test_PluginPluginGUI()
    : dspCode(L"")
    , faustWnd(nullptr)
    , editorWnd(nullptr)
    , audioInputCombo(nullptr)
    , currAudioInputComboSelection(DEFAUT_AUDIO_INPUT_FOR_EFFECTS)
    , pluginWindow(nullptr)
    , state(WM_STATE::NONINIT_STATE)
    , UnqBuildCompleteWndMsg(RegisterWindowMessage(L"FaustBuildCompleteMessage"))
{
    std::string exportPath = faustPluginLoader.getExportPath();
    codePath = PluginUtils::string2wstring(exportPath + "/" + std::string(LASTCODE_FNAME));
    entry_code = PluginUtils::string2wstring( std::string(default_entry_code));
    
    AKPLATFORM::OutputDebugMsg("Faust :: Constructor got called!\n");
}

Faust_Interpreter_Test_PluginPluginGUI::~Faust_Interpreter_Test_PluginPluginGUI()
{   
    if (pluginWindow)
    {   
        delete pluginWindow;
        pluginWindow = nullptr;
    }

}

HINSTANCE Faust_Interpreter_Test_PluginPluginGUI::GetResourceHandle() const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return AfxGetStaticModuleState()->m_hCurrentResourceHandle;
}

bool Faust_Interpreter_Test_PluginPluginGUI::GetDialog(
    AK::Wwise::Plugin::eDialog in_eDialog,
    UINT& out_uiDialogID,
    AK::Wwise::Plugin::PopulateTableItem*& out_pTable) const
{
    if (in_eDialog == AK::Wwise::Plugin::SettingsDialog)
    {
        out_uiDialogID = IDD_FAUST_PLUGIN;
        out_pTable = PropertyTable;
        return true;
    }
    AKPLATFORM::OutputDebugMsg("Faust :: GetDialog got called!\n");
    return false;
}

bool Faust_Interpreter_Test_PluginPluginGUI::WindowProc(
    AK::Wwise::Plugin::eDialog in_eDialog,
    HWND in_hWnd,
    uint32_t in_message,
    WPARAM in_wParam,
    LPARAM in_lParam,
    LRESULT& out_lResult)
{

    switch (state)
    {
        case INIT_STATE:
        {
            SetCodeEditorText();
            state = WM_STATE::RUN_STATE;
        }
    }

    if (in_message == UnqBuildCompleteWndMsg)   // UnqBuildCompleteWndMsg is runtime evaluated. Have to be within an if condition instead of being within a case statement.
    {
        
        EnableWindow((HWND)in_wParam, TRUE); // re-enable build button after completion

        bool succeeded = (bool)in_lParam;
        std::wstring outputMessage;
        if (succeeded) 
        {
            outputMessage = L"Plugin Build Successfully!\n" + PluginUtils::string2wstring(buildOutputText);
        } else 
        {
            outputMessage = std::wstring(L"Failed to build.\nFor more details check the output log file:\n") 
                            + PluginUtils::string2wstring(buildOutputText);
        }

        MessageBoxW(faustWnd, outputMessage.c_str(), L"Faust2Wwise Build Result", MB_OK);
        return TRUE;
    }

    BOOL bRet = FALSE;
    switch (in_message)
    {
        case WM_INITDIALOG:
        {
            AKPLATFORM::OutputDebugMsg("Faust :: INITDIALOG got called!\n");
            faustWnd = in_hWnd;
            editorWnd = GetDlgItem(faustWnd, IDC_CODE_EDITOR);
            audioInputCombo = GetDlgItem(faustWnd, IDC_AUDIO_INPUT_COMBO);

            audioInputList audlist = AudioInputs::getAudioInputList();
            for (auto& [id, name] : audlist)
            {
                SendMessageW(audioInputCombo, CB_ADDSTRING, id, (LPARAM)name);
            }
            SendMessageW(audioInputCombo, CB_SETCURSEL, currAudioInputComboSelection, 0);
            state = WM_STATE::INIT_STATE;
            bRet=TRUE;
            break;  
        }
        case WM_COMMAND:
        {
            int id = LOWORD(in_wParam);
            int notify = HIWORD(in_wParam);
            
            if (notify == BN_CLICKED)
            {
                if (id == IDC_BUTTON_PREVIEW)
                {
                    //@TODO Pass the name of the Plugin  - > create a field for defining the plugin name, or maybe a fileloader just like the online editor.
                    SaveCodeEditorText();
                    OnPreviewButtonClicked();
                }
                if (id == IDC_BUTTON_BUILD)
                {
                    SaveCodeEditorText();
                    OnBuildButtonClicked();
                }
            }
            else if (notify == CBN_SELCHANGE)
            {
                if (id == IDC_AUDIO_INPUT_COMBO)
                {
                    int selection = (int)SendMessageW(audioInputCombo, CB_GETCURSEL, 0, 0);
                    currAudioInputComboSelection = selection;
                }
            } 
            bRet = TRUE;
            break;
        }
        case WM_DESTROY:
        {
            AKPLATFORM::OutputDebugMsg("Faust :: WM_DESTROY got called!\n");
            faustPluginLoader.unloadPlugin();    
            SaveCodeEditorText();
            state = WM_STATE::EXIT_STATE;
            bRet = TRUE;
            break;
        }
    }
    return bRet;
}

bool Faust_Interpreter_Test_PluginPluginGUI::SetCodeEditorText()
{
    if (!editorWnd) return false;
    
    std::wstring text;
    if (loadLastSavedCode())
        text = dspCode;
    else{
        text = PluginUtils::string2wstring(std::string(default_entry_code));
    }
    SetWindowTextW(editorWnd, text.c_str());
    return true;
}

bool Faust_Interpreter_Test_PluginPluginGUI::SaveCodeEditorText()
{
    int len = GetWindowTextLengthW(editorWnd);
    if (len <= 0)
        return false;

    std::wstring buffer(len, L'\0');
    GetWindowTextW(editorWnd, &buffer[0], len + 1);

    // ensure null-termination and trim
    buffer.resize(wcslen(buffer.c_str()));

    dspCode = buffer;

    saveCurrentCodeState();

    return true;
}

void Faust_Interpreter_Test_PluginPluginGUI::OnPreviewButtonClicked()
{   
    // Important: this function runs asynchronously.
    
    PluginState pluginState = faustPluginLoader.getPluginState();
    if ( pluginState != PluginState::READY)
    {

        // build plugin asynchronously
        std::thread([this]() {
                
                bool pluginCreated = faustPluginLoader.createPlugin(
                    PluginUtils::wstring2string(this->dspCode),
                    currAudioInputComboSelection
                );
                
                if (!pluginCreated)
                    return;
                    
                // disable selection of audio input source for the effect plugins when preview.
                EnableWindow(audioInputCombo, FALSE);

                ParameterList &parameters = faustPluginLoader.getParameters();
                PluginConfiguration& cfg = faustPluginLoader.getConfiguration();
                PluginState pluginState = faustPluginLoader.getPluginState();

                // verify that params are obtained...
                if (parameters.empty())
                {
                    ShowEmptyParametersWindow(PluginUtils::string2wstring( cfg.plugin_name ).c_str());

                    while(pluginState==PluginState::PLUGIN_SET || pluginState==PluginState::READY)
                    {
                        pluginState = faustPluginLoader.getPluginState();
                    }
                }
                else
                {                
                    pluginWindow = new PluginWindow(faustWnd,cfg,parameters);
                    if (pluginWindow->isWindowCreated())
                    {
                        pluginWindow->Show();
                        pluginState = faustPluginLoader.getPluginState();
                        while
                        ( 
                            (pluginState==PluginState::PLUGIN_SET || pluginState==PluginState::READY)
                            && pluginWindow 
                            && pluginWindow->isActive() 
                        )
                        {
                            pluginWindow->Update();
                            pluginState = faustPluginLoader.getPluginState();
                        }
                        // close PluginWindow and reset plugin
                        if (pluginWindow){
                            delete pluginWindow;
                            pluginWindow = nullptr;
                            faustPluginLoader.resetPlugin();
                        }
                    }
                }
            
                // re-enable the audio Input combo
                EnableWindow(audioInputCombo, TRUE);

        }).detach();

    }

}

void Faust_Interpreter_Test_PluginPluginGUI::OnBuildButtonClicked(){
    
    HWND buildButton = GetDlgItem(faustWnd, IDC_BUTTON_BUILD);
    EnableWindow(buildButton, FALSE);   // disable build button...

    std::thread([this, buildButton]()
    {        
        // compile using faust2wwise implementation..
        bool res = faustPluginLoader.buildPlugin(PluginUtils::wstring2string(dspCode), buildOutputText);
        
        // temp log message
        char dbg[256];
        snprintf(dbg, 256, "OnBuildButtonClicked: result is %d", res);
        AKPLATFORM::OutputDebugMsg(dbg);

        // notify GUI thread - re-enabling the button will happen after the message is received within winProc function.
        PostMessage(faustWnd, UnqBuildCompleteWndMsg, (WPARAM)buildButton, (LPARAM)res);

        //@TODO Design a way to allow user to inspect the output of the build (errors/warnings/success etc..)

    }).detach();

}

void Faust_Interpreter_Test_PluginPluginGUI::ShowEmptyParametersWindow(LPCWSTR pluginName)
{
    MessageBoxW(faustWnd, L"The DSP code does not define any parameters. Simply press \'Play\' to start playback", pluginName , MB_OK | MB_ICONINFORMATION);
}

bool Faust_Interpreter_Test_PluginPluginGUI::loadLastSavedCode()
{

    FILE* file = _wfopen(codePath.c_str(), L"rb");  // binary mode to preserve UTF-16 encoding
    if (!file) return false;

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size % sizeof(wchar_t) != 0 || size <= 0) {
        fclose(file);
        return false;
    }

    // Read file into buffer
    size_t wcharCount = size / sizeof(wchar_t);
    std::wstring buffer(wcharCount, L'\0');

    fread(&buffer[0], sizeof(wchar_t), wcharCount, file);
    fclose(file);

    dspCode = buffer;
    return true;
}

bool Faust_Interpreter_Test_PluginPluginGUI::saveCurrentCodeState()
{

    FILE* file = _wfopen(codePath.c_str(), L"wb");  // binary mode to preserve encoding
    if (!file) return false;

    fwrite(dspCode.c_str(), sizeof(wchar_t), dspCode.size(), file);
    fclose(file);
    return true;
}

void Faust_Interpreter_Test_PluginPluginGUI::debugPrint(std::wstring text, size_t len)
{
    // Output debug using narrow version just for logging
    std::string narrowDebug;
    {
        int required = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (required > 0) {
            narrowDebug.resize(required - 1); // exclude null terminator
            WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, &narrowDebug[0], required, nullptr, nullptr);
        }
    }

    char output[512];
    snprintf(output, sizeof(output),
        "Faust :: text debug print (len=%d) = %s\n",
        len, narrowDebug.c_str());

    AKPLATFORM::OutputDebugMsg(output);
}

AK_ADD_PLUGIN_CLASS_TO_CONTAINER(
    Faust_Interpreter_Test_Plugin,            // Name of the plug-in container for this shared library
    Faust_Interpreter_Test_PluginPluginGUI,   // Authoring plug-in class to add to the plug-in container
    Faust_Interpreter_Test_PluginSource       // Corresponding Sound Engine plug-in class
);
