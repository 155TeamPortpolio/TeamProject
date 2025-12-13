#pragma once
#include "AnimationTool_Defines.h"
#include "AnimationClip.h"

NS_BEGIN(Engine)

class CAnimationClipEX :
	public CAnimationClip
{
protected:
	CAnimationClipEX(const string& Subject);
	CAnimationClipEX();
	virtual ~CAnimationClipEX() DEFAULT;

public:
	HRESULT Initialize(const string& animationPath);
	_float Get_Duration() { return m_fDuration; }
	_bool isLoop() { return m_bLoop; }

	//현재 애니매이션 클립을 재생함
	_float TranslateAnimateMatrix(vector<_float4x4>& transfomationMatrices,
		_float CurrentTrackPosition,
		_float dt,
		_bool isLoop,
		_bool* isAnimEnd);

	//현재 키프레임을 기준으로 다음 애니매이션을 보간
	_bool ConvertTo(vector<_float4x4>& transfomationMatrices,
		CAnimationClip& DestAnimation,
		_float fConvertDuration,
		_float PrevTrackPosition,
		_float ConversionTrackPosition);

	//현재 트랙포지션 기준으로 다음 애니매이션을 보간
	_bool ConvertByCurrentMatrix(
		vector<_float4x4>& transfomationMatrices,
		CAnimationClip& DestAnimation,
		_float fConvertDuration,
		_float PrevTrackPosition,
		_float ConversionTrackPosition);

public:
	class CChannel* Find_ChannelByBoneName(const string& boneName);

	const string& Get_Name() { return m_ClipName; }
	const string& Get_Subject() { return m_Subject; }
	_float Get_TickPerSecond() { return m_fTickPerSecond; }

public:
	virtual void Render_GUI();

protected:
	_bool					m_bLoop = { false };	//반복 여부
	_float					m_fDuration = {};		//총 재생
	_float					m_fTickPerSecond = {};	//속도
	_uint					m_iNumChannels = {};	//채널 개수
	string					m_ClipName = {};		//애니매이션 클립 이름
	string					m_Subject = {};			//어떤 모델의 애니매이션인지?
	vector<class CChannel*> m_Channels;

public:
	static CAnimationClipEX* Create(const string& animationPath, const string& animClipKey, const string& Subject);
	virtual void Free();
};
NS_END
