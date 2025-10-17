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

#include "Faust_Interpreter_Test_PluginSource.h"
#include "../Faust_Interpreter_Test_PluginConfig.h"
#include <AK/AkWwiseSDKVersion.h>
#include <string>

AK::IAkPlugin* CreateFaust_Interpreter_Test_PluginSource(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, Faust_Interpreter_Test_PluginSource());
}

AK::IAkPluginParam* CreateFaust_Interpreter_Test_PluginSourceParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, Faust_Interpreter_Test_PluginSourceParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(Faust_Interpreter_Test_PluginSource, AkPluginTypeSource, Faust_Interpreter_Test_PluginConfig::CompanyID, Faust_Interpreter_Test_PluginConfig::PluginID)

Faust_Interpreter_Test_PluginSource::Faust_Interpreter_Test_PluginSource()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

Faust_Interpreter_Test_PluginSource::~Faust_Interpreter_Test_PluginSource()
{
}

AKRESULT Faust_Interpreter_Test_PluginSource::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkSourcePluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (Faust_Interpreter_Test_PluginSourceParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

    m_durationHandler.Setup(m_pParams->RTPC.fDuration, in_pContext->GetNumLoops(), in_rFormat.uSampleRate);

    return AK_Success;
}

AKRESULT Faust_Interpreter_Test_PluginSource::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT Faust_Interpreter_Test_PluginSource::Reset()
{
    return AK_Success;
}

AKRESULT Faust_Interpreter_Test_PluginSource::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeSource;
    out_rPluginInfo.bIsInPlace = true;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void Faust_Interpreter_Test_PluginSource::Execute(AkAudioBuffer* out_pBuffer)
{
    m_durationHandler.SetDuration(m_pParams->RTPC.fDuration);
    m_durationHandler.ProduceBuffer(out_pBuffer);

    const AkUInt32 uNumChannels = out_pBuffer->NumChannels();

    AkUInt16 uFramesProduced;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)out_pBuffer->GetChannel(i);

        uFramesProduced = 0;
        while (uFramesProduced < out_pBuffer->uValidFrames)
        {
            // Generate output here
            *pBuf++ = 0.0f;
            ++uFramesProduced;
        }
    }

    DLLState dllstate = myFaustPlugin.getPluginState();
    if (dllstate == DLL_COMPILED) // @TODO change to DLL_LINKED after implementing the linking stage.
    {
        AKPLATFORM::OutputDebugMsg("Execute function can see that the dll is compiled!");
    }
    else if (dllstate == LINKED)
    {
        AKPLATFORM::OutputDebugMsg("Execute function can see that the dll is linked!");
    }

}

AkReal32 Faust_Interpreter_Test_PluginSource::GetDuration() const
{
    return m_durationHandler.GetDuration() * 1000.0f;
}
