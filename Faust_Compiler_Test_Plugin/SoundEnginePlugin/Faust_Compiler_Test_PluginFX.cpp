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

  Copyright (c) 2021 Audiokinetic Inc.
*******************************************************************************/

#include "Faust_Compiler_Test_PluginFX.h"
#include "../Faust_Compiler_Test_PluginConfig.h"

#include <AK/AkWwiseSDKVersion.h>

#include <string>

static const std::string script = "import(\"stdfaust.lib\"); process = no.noise;";

AK::IAkPlugin* CreateFaust_Compiler_Test_PluginFX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, Faust_Compiler_Test_PluginFX());
}

AK::IAkPluginParam* CreateFaust_Compiler_Test_PluginFXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, Faust_Compiler_Test_PluginFXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(Faust_Compiler_Test_PluginFX, AkPluginTypeEffect, Faust_Compiler_Test_PluginConfig::CompanyID, Faust_Compiler_Test_PluginConfig::PluginID)

Faust_Compiler_Test_PluginFX::Faust_Compiler_Test_PluginFX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

Faust_Compiler_Test_PluginFX::~Faust_Compiler_Test_PluginFX()
{
}

AKRESULT Faust_Compiler_Test_PluginFX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (Faust_Compiler_Test_PluginFXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

    int argc = 0;
    const char* argv[8];
    argv[argc++] = "-I";
    argv[argc] = nullptr;  // NULL terminated argv

    std::string error_msg;
    std::string target;
    m_DSPFactory = createInterpreterDSPFactoryFromString("FaustDSP", script, argc, argv, error_msg);

    return AK_Success;
}

AKRESULT Faust_Compiler_Test_PluginFX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT Faust_Compiler_Test_PluginFX::Reset()
{
    return AK_Success;
}

AKRESULT Faust_Compiler_Test_PluginFX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = true;
	out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void Faust_Compiler_Test_PluginFX::Execute(AkAudioBuffer* io_pBuffer)
{
    const AkUInt32 uNumChannels = io_pBuffer->NumChannels();

    AkUInt16 uFramesProcessed;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)io_pBuffer->GetChannel(i);

        uFramesProcessed = 0;
        while (uFramesProcessed < io_pBuffer->uValidFrames)
        {
            // Execute DSP in-place here
            ++uFramesProcessed;
        }
    }
}

AKRESULT Faust_Compiler_Test_PluginFX::TimeSkip(AkUInt32 in_uFrames)
{
    return AK_DataReady;
}
