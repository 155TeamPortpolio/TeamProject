#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAnimationClip :
    public CBase
{
protected:
    CAnimationClip(const string& Subject);
    CAnimationClip();
    virtual ~CAnimationClip() DEFAULT;

public:
	HRESULT Initialize(const string& animationPath);
	_float Get_Duration() { return m_fDuration; }
	_bool isLoop() { return m_bLoop; }

	_float TranslateAnimateMatrix( vector<_float4x4>& transfomationMatrices,
		_float CurrentTrackPosition,
		_float dt,
		_bool isLoop,
		_bool* isAnimEnd);

	_bool ConvertTo(vector<_float4x4>& transfomationMatrices,
		 CAnimationClip& DestAnimation, 
		_float fConvertDuration, 
		_float PrevTrackPosition, 
		_float ConversionTrackPosition);

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
	_bool					m_bLoop = { false };
	_float					m_fDuration = {}; //ÃÑ Àç»ý
	_float					m_fTickPerSecond = {}; //¼Óµµ
	_uint					m_iNumChannels = {};
	string				m_ClipName = {};
	string				m_Subject = {};
	vector<class CChannel*> m_Channels;

public:
    static CAnimationClip* Create(const string& animationPath, const string& animClipKey, const string& Subject);
	virtual void Free();
};
NS_END
