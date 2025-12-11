#include "Engine_Defines.h"
#include "AnimationClip.h"
#include "Channel.h"

CAnimationClip::CAnimationClip(const string& Subject)
	:m_Subject(Subject)
{
}

CAnimationClip::CAnimationClip()
{
}

HRESULT CAnimationClip::Initialize(const string& animationPath)
{
	ifstream ifs(animationPath.c_str(), ios::binary);

	if (!ifs.is_open()) {
		string msg = "Anim Add Failed path: " + animationPath+"\n";
		OutputDebugStringA(msg.c_str());
		return E_FAIL;
	}

	ANIMATION_CLIP_HEADER ClipHeader = {};
	ifs.read(reinterpret_cast<char*>(&ClipHeader), sizeof(ANIMATION_CLIP_HEADER));

	m_bLoop = ClipHeader.bLoop;
	m_ClipName = ClipHeader.ClipName;
	m_fDuration = ClipHeader.fDuration;
	m_fTickPerSecond = ClipHeader.fTickPerSecond;
	m_iNumChannels= ClipHeader.iNumChannels;
	for (size_t i = 0; i < ClipHeader.iNumChannels; i++)
	{
		if (CChannel* pChannel = CChannel::Create(ifs))
			if(pChannel)
			m_Channels.push_back(pChannel);
	}

	ifs.close();
	return S_OK;
}

_float CAnimationClip::TranslateAnimateMatrix(vector<_float4x4>& transfomationMatrices, _float CurrentTrackPosition, _float dt, _bool isLoop, _bool* isAnimEnd)
{

	_float RealTrackPosition = CurrentTrackPosition + dt * m_fTickPerSecond;

	if (isLoop) {
		if (RealTrackPosition > m_fDuration)
			return 0;
	}
	else {
		if (RealTrackPosition > m_fDuration) {
			RealTrackPosition = m_fDuration; 
			*isAnimEnd = true;
		}
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->TranslateAnimateMatrix(transfomationMatrices, RealTrackPosition, isLoop);
	}

	return RealTrackPosition;
}

_bool CAnimationClip::ConvertTo(vector<_float4x4>& transfomationMatrices, CAnimationClip& DestAnimation, _float fConvertDuration, _float PrevTrackPosition, _float ConversionTrackPosition)
{

	for (auto& SrcChannel : m_Channels) {
		CChannel* nextChannel = DestAnimation.Find_ChannelByBoneName(SrcChannel->Get_Name());
		
		SrcChannel->ConvertAnimateMatrix(
			transfomationMatrices, 
			fConvertDuration, 
			PrevTrackPosition, 
			ConversionTrackPosition,
			nextChannel);
	}

	if (fConvertDuration <= ConversionTrackPosition)
		return true;
	else
		return false;
}

_bool CAnimationClip::ConvertByCurrentMatrix(vector<_float4x4>& transfomationMatrices, CAnimationClip& DestAnimation, _float fConvertDuration, _float PrevTrackPosition, _float ConversionTrackPosition)
{
	for (auto& SrcChannel : m_Channels) {
		CChannel* nextChannel = DestAnimation.Find_ChannelByBoneName(SrcChannel->Get_Name());

		SrcChannel->ConvertByCurrentMatrix(
			transfomationMatrices,
			fConvertDuration,
			PrevTrackPosition,
			ConversionTrackPosition,
			nextChannel);
	}

	if (fConvertDuration <= ConversionTrackPosition)
		return true;
	else
		return false;
}

CChannel* CAnimationClip::Find_ChannelByBoneName(const string& boneName)
{
	auto iter = find_if(m_Channels.begin(), m_Channels.end(), [&](CChannel* pChannel)->bool {
		return boneName == pChannel->Get_Name();
		});

	if (iter != m_Channels.end()) {
		return *iter;
	}
	else {
		return nullptr;
	}
}

void CAnimationClip::Render_GUI()
{
	for (auto& channel : m_Channels)
		channel->Render_GUI();
}

CAnimationClip* CAnimationClip::Create(const string& animationPath, const string& animClipKey, const string& Subject)
{
	CAnimationClip* instance = new CAnimationClip(Subject);
	if (FAILED(instance->Initialize(animationPath))) {
		Safe_Release(instance);
	}
	return instance;
}

void CAnimationClip::Free()
{
	__super::Free();

	for (auto& channel : m_Channels)
		Safe_Release(channel);
}
