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
	//현재 애니매이션 클립을 재생함
	_float TranslateAnimateMatrix( vector<_float4x4>& transfomationMatrices,
		_float CurrentTrackPosition,
		_float dt,
		_bool isLoop,
		_bool* isAnimEnd);

	void TranslateAnimateMatrixFromDuration(vector<_float4x4>& transfomationMatrices, _float CurrentTrackPosition);

public:
	class CChannel* Find_ChannelByBoneName(const string& boneName);
	_bool isLoop() { return m_bLoop; }
	_float Get_TickPerSec() { return m_fTickPerSecond; }
	_float Get_Duration() { return m_fDuration; }
	const string& Get_Name() { return m_ClipName; }

public:
	virtual void Render_GUI();

protected:
	_bool					m_bLoop = { false };	//반복 여부
	_float					m_fDuration = {};		//총 재생
	_float					m_fTickPerSecond = {};	//속도
	_uint					m_iNumChannels = {};	//채널 개수
	string					m_ClipName = {};		//애니매이션 클립 이름
	vector<class CChannel*> m_Channels;

public:
    static CAnimationClip* Create(const string& animationPath, const string& animClipKey);
	virtual void Free();
};
NS_END
