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
#include "syscall.h"
#include <AK/AkWwiseSDKVersion.h>
#include <windows.h>
#include <sstream>
#include <locale>
#include <codecvt>
#include <thread>

inline std::string wstring2string(const std::wstring wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

inline std::wstring string2wstring(const std::string str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);   
}

AK_WWISE_PLUGIN_GUI_WINDOWS_BEGIN_POPULATE_TABLE(PropertyTable)
AK_WWISE_PLUGIN_GUI_WINDOWS_END_POPULATE_TABLE()

Faust_Interpreter_Test_PluginPluginGUI::Faust_Interpreter_Test_PluginPluginGUI()
    : dspCode(L"")
    , faustWnd(nullptr)
    , editorWnd(nullptr)
    , state(NONINIT_STATE)
{
    wwiseRoot = SysCall::getEnvVar("WWISEROOT");
    tempDir = std::filesystem::path(wwiseRoot) / "_f2wTemp_";
    if (!std::filesystem::exists(tempDir)) {
        std::filesystem::create_directory(tempDir);
    }

    faustInterpreter.set_exportPath(tempDir.string());

    entry_code = string2wstring(faustInterpreter.get_default_entry_code());
    AKPLATFORM::OutputDebugMsg("Faust :: Constructor got called!\n");
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
    // AKPLATFORM::OutputDebugMsg("Faust :: GetDialog got called!\n");
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
    BOOL bRet = FALSE;

    switch (state)
    {
        case INIT_STATE:
        {
            SetCodeEditorText();
            state = RUN_STATE;
        }
    }
    
    switch (in_message)
    {
        case WM_INITDIALOG:
        {
            // AKPLATFORM::OutputDebugMsg("Faust :: INITDIALOG got called!\n");
            faustWnd = in_hWnd;
            editorWnd = GetDlgItem(faustWnd, IDC_CODE_EDITOR);
            state = INIT_STATE;
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
                    OnPreviewButtonClicked();
                }
                if (id == IDC_BUTTON_BUILD)
                {
                    OnBuildButtonClicked();
                }
            }    
            bRet = TRUE;
            break;
        }
        case WM_DESTROY:
        {
            AKPLATFORM::OutputDebugMsg("Faust :: WM_DESTROY got called!\n");
            SaveCodeEditorText();
            faustWnd = editorWnd = nullptr;
            state = EXIT_STATE;
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
        text = string2wstring(faustInterpreter.get_default_entry_code());
    }
    SetWindowTextW(editorWnd, text.c_str());
    return true;
}

bool Faust_Interpreter_Test_PluginPluginGUI::SaveCodeEditorText()    // rename that to GetCodeEditorText
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

bool Faust_Interpreter_Test_PluginPluginGUI::OnPreviewButtonClicked()
{   
    // Important: this function runs asynchronously.
    
    if (SaveCodeEditorText()){

        // preview not supported yet
        ShowPopupWindow();

        // build plugin dll asynchronously
        std::thread([this]() {

                faust_plugin.setPluginState(DLLState::PENDING_COMPILE);
                
                bool dspCompiled = faustInterpreter.compileDSP(wstring2string(this->dspCode));
                if (!dspCompiled)
                {
                    faust_plugin.setPluginState(DLLState::FAIL_COMPILE_DSP);   
                    return;
                }

                bool exported = faustInterpreter.exportCPP();
                if (!exported)
                {
                    faust_plugin.setPluginState(DLLState::FAIL_EXPORT_CPP);   
                    return;
                }
                
                bool compiled = faustInterpreter.compileCPP();
                if (!compiled)
                {    
                    faust_plugin.setPluginState(DLLState::FAIL_COMPILE_CPP);   
                    return;
                }

                faust_plugin.setPluginState(DLLState::DLL_COMPILED);
                faust_plugin.loadDynamicLib(faustInterpreter.get_dll_path());

        }).detach();

        return true; // return the result of the faust2wwise dynamic compilation..
    }
    return false;
}

bool Faust_Interpreter_Test_PluginPluginGUI::OnBuildButtonClicked(){

    if (SaveCodeEditorText()) {

        // saves the code..
        // and now try to compile using faustInterpreter object..
        bool res = faustInterpreter.buildPlugin(wstring2string(dspCode));
        char dbg[256];
        snprintf(dbg, 256, "OnBuildButtonClicked: result is %d", res);
        AKPLATFORM::OutputDebugMsg(dbg);

        //@TODO Design a way to allow user to inspect the output of the build (errors/warnings/success etc..)
    }   
    return false;
}

void Faust_Interpreter_Test_PluginPluginGUI::ShowPopupWindow()
{
    MessageBoxW(faustWnd, L"Output not yet supported!", L"FAUST Code Output", MB_OK | MB_ICONINFORMATION);
}


bool Faust_Interpreter_Test_PluginPluginGUI::loadLastSavedCode()
{
    std::filesystem::path codePath = tempDir / "last_code.dsp";

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
    std::filesystem::path codePath = tempDir / "last_code.dsp";

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
