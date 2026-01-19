#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAnimationClip :
    public CBase
{
protected:
    CAnimationClip();
    virtual ~CAnimationClip() DEFAULT;

public:
	HRESULT Initialize(const string& animationPath);
	void Set_Events(vector<ANIM_EVENT>& Events);

	//현재 애니매이션 클립을 재생함
	_float TranslateAnimateMatrix(vector<_float4x4>& transfomationMatrices,
		_float CurrentTrackPosition,
		_float dt,
		_bool isLoop, _float fEndAt, _float fStartAt,
		_bool* isWarpped,
		_bool* isAnimEnd,
		_float* outProgress,
		vector<EVENT_INST>& EventBus);

	void TranslateAnimateMatrixFromDuration(vector<_float4x4>& transfomationMatrices,
		_float TrackPosition,
		vector<EVENT_INST>& EventBus);

	void TranslateAnimateMatrixFromDurationNoEvent(vector<_float4x4>& transfomationMatrices,
		_float Duration);

	void SampleKayFrame(_int iBoneIndex, _float fProgress,
		_vector3* pOutScale, _quaternion* pOutQuat, _vector3* pOutPos);

public:
	class CChannel* Find_ChannelByBoneName(const string& boneName);
	_bool isLoop() { return m_bLoop; }
	_float Get_TickPerSec() { return m_fTickPerSecond; }
	_float Get_Duration() { return m_fDuration; }
	const string& Get_Name() { return m_ClipName; }
	const vector<ANIM_EVENT>& Get_Events() { return m_Events; }
	void Check_Event(_float PrevTrackPos, _float CurTrackPos, vector<EVENT_INST>& EventBus);

	const KEYFRAME& Get_StartKeyFrameByBoneName(const string& BoneName) const;
	const KEYFRAME& Get_StartKeyFrameByBoneIndex(_uint BoneIndex) const;
	const KEYFRAME& Get_EndKeyFrameByBoneName(const string& BoneName) const;
	const KEYFRAME& Get_EndKeyFrameByBoneIndex(_uint BoneIndex) const;

	void Sample_KeyFrameByBoneName(const string& BoneName, const _float fProgress, _vector3* pOutScale, _quaternion* pOutQuat, _vector3* pOutTrans) const;
	void Sample_KeyFrameByBoneIndex(_uint BoneIndex, const _float fProgress, _vector3* pOutScale, _quaternion* pOutQuat, _vector3* pOutTrans) const;

public:
	virtual void Render_GUI();

protected:
	_bool					m_bLoop = { false };	//반복 여부
	_float					m_fDuration = {};		//총 재생
	_float					m_fTickPerSecond = {};	//속도
	_uint					m_iNumChannels = {};	//채널 개수
	string					m_ClipName = {};		//애니매이션 클립 이름
	vector<class CChannel*> m_Channels;
	vector<ANIM_EVENT>		m_Events;

public:
    static CAnimationClip* Create(const string& animationPath);
	virtual void Free();
};
NS_END
