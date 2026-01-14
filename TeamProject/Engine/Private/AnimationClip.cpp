#include "Engine_Defines.h"
#include "AnimationClip.h"
#include "AnimationLayout.h"
#include "Channel.h"

CAnimationClip::CAnimationClip()
{
}

HRESULT CAnimationClip::Initialize(const string& animationPath)
{
	ifstream ifs(animationPath.c_str(), ios::binary);

	if (!ifs.is_open()) {
		string msg = "Anim Add Failed path: " + animationPath + "\n";
		OutputDebugStringA(msg.c_str());
		return E_FAIL;
	}

	ANIMATION_CLIP_HEADER ClipHeader = {};
	ifs.read(reinterpret_cast<char*>(&ClipHeader), sizeof(ANIMATION_CLIP_HEADER));

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

void CAnimationClip::Set_Events(vector<ANIM_EVENT>& Events)
{
	m_Events = Events;
}

_float CAnimationClip::TranslateAnimateMatrix(vector<_float4x4>& transfomationMatrices,
	_float CurrentTrackPosition, _float dt, _bool isLoop,
	_bool* isWarpped, _bool* isAnimEnd, _float* outProgress,
	vector<EVENT_INST>& EventBus)
{
	_float RealTrackPosition = CurrentTrackPosition + dt * m_fTickPerSecond;

	if (isLoop) {
		*isWarpped = false;
		if (RealTrackPosition > m_fDuration) {
			RealTrackPosition -= m_fDuration;
			*isWarpped = true;
		}
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
	
	Check_Event(CurrentTrackPosition, RealTrackPosition, EventBus);

	if(outProgress)
		*outProgress = RealTrackPosition / m_fDuration;

	return RealTrackPosition;
}

void CAnimationClip::TranslateAnimateMatrixFromDuration(vector<_float4x4>& transfomationMatrices,
	_float TrackPosition, vector<EVENT_INST>& EventBus)
{
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->TranslateAnimateMatrix(transfomationMatrices, TrackPosition, false);
	}

	Check_Event(0, TrackPosition, EventBus);
}

void CAnimationClip::TranslateAnimateMatrixFromDurationNoEvent(vector<_float4x4>& transfomationMatrices, _float Duration)
{
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->TranslateAnimateMatrix(transfomationMatrices, Duration, false);
	}
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

void CAnimationClip::Check_Event(_float PrevTrackPos, _float CurTrackPos, vector<EVENT_INST>& EventBus)
{
	for (auto& Event : m_Events)
	{
		//이전 프레임엔 작았고 현재프레임엔 크면
		if (PrevTrackPos < Event.EventTime && Event.EventTime <= CurTrackPos)
		{
			EventBus.emplace_back(EVENT_INST{ Event.EventType, Event.EventTag });
		}
	}
}

const KEYFRAME& CAnimationClip::Get_StartKeyFrameByBoneName(const string& BoneName) const
{
	for (auto Channel : m_Channels)
		if (BoneName == Channel->Get_Name())
			return Channel->Get_KeyFrames().front();

	return KEYFRAME{};
}

const KEYFRAME& CAnimationClip::Get_StartKeyFrameByBoneIndex(_uint BoneIndex) const
{
	for (auto Channel : m_Channels)
		if (BoneIndex == Channel->Get_BoneIndex())
			return Channel->Get_KeyFrames().front();

	return KEYFRAME{};
}

const KEYFRAME& CAnimationClip::Get_EndKeyFrameByBoneName(const string& BoneName) const
{
	for (auto Channel : m_Channels)
		if (BoneName == Channel->Get_Name())
			return Channel->Get_KeyFrames().back();

	return KEYFRAME{};
}

const KEYFRAME& CAnimationClip::Get_EndKeyFrameByBoneIndex(_uint BoneIndex) const
{
	for (auto Channel : m_Channels)
		if (BoneIndex == Channel->Get_BoneIndex())
			return Channel->Get_KeyFrames().back();

	return KEYFRAME{};
}


void CAnimationClip::Render_GUI()
{
	for (auto& channel : m_Channels)
		channel->Render_GUI();
}

CAnimationClip* CAnimationClip::Create(const string& animationPath)
{
	CAnimationClip* instance = new CAnimationClip();
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

	m_Events.clear();
}
