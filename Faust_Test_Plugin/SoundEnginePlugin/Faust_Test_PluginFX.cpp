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

  Copyright (c) 2006-2022 Audiokinetic Inc.
*******************************************************************************/

#include "Faust_Test_PluginFX.h"
#include "../Faust_Test_PluginConfig.h"

#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin* CreateFaust_Test_PluginFX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, Faust_Test_PluginFX());
}

AK::IAkPluginParam* CreateFaust_Test_PluginFXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, Faust_Test_PluginFXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(Faust_Test_PluginFX, AkPluginTypeEffect, Faust_Test_PluginConfig::CompanyID, Faust_Test_PluginConfig::PluginID)

Faust_Test_PluginFX::Faust_Test_PluginFX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

Faust_Test_PluginFX::~Faust_Test_PluginFX()
{
}

AKRESULT Faust_Test_PluginFX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (Faust_Test_PluginFXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

    m_dsp[0].init(static_cast<int>(in_rFormat.uSampleRate));
    m_dsp[1].init(static_cast<int>(in_rFormat.uSampleRate));

    return AK_Success;
}

AKRESULT Faust_Test_PluginFX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT Faust_Test_PluginFX::Reset()
{
    return AK_Success;
}

AKRESULT Faust_Test_PluginFX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = true;
	out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void Faust_Test_PluginFX::Execute(AkAudioBuffer* io_pBuffer)
{
    const AkUInt32 uNumChannels = io_pBuffer->NumChannels();

    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        m_dsp[i].setDelayTime(m_pParams->RTPC.fDelayTime);
        m_dsp[i].setFeedback(m_pParams->RTPC.fFeedback);
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)io_pBuffer->GetChannel(i);
        m_dsp[i].compute(static_cast<int>(io_pBuffer->uValidFrames), &pBuf, &pBuf);
    }
}

AKRESULT Faust_Test_PluginFX::TimeSkip(AkUInt32 in_uFrames)
{
    return AK_DataReady;
}
