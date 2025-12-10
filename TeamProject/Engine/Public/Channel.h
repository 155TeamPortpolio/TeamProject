#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CChannel :
    public CBase
{
protected:
    CChannel();
    virtual ~CChannel() DEFAULT;

public:
    HRESULT Initialize(ifstream& ifs);
    virtual HRESULT TranslateAnimateMatrix( vector<_float4x4>& transfomationMatrices, _float CurrentTrackPosition , _bool Loop);
    virtual HRESULT ConvertAnimateMatrix(vector<_float4x4>& transfomationMatrices, _float ConvertDuration, _float PrevTrackPosition,_float ConversionTrackPosition, CChannel*  NextChannel);
    virtual HRESULT ConvertByCurrentMatrix(vector<_float4x4>& transfomationMatrices, _float ConvertDuration, _float PrevTrackPosition,_float ConversionTrackPosition, CChannel*  NextChannel);

    const string& Get_Name() { return m_ChannelName; }

public:
    virtual void Render_GUI();

public:
    vector<KEYFRAME>& Get_KeyFrames() { return m_KeyFrames; };

protected:
    _bool             m_isRoot = {};
    string			m_ChannelName;       //본과 일치할 수 있도록.
    _uint				m_iBoneIndex = {};
    _uint				m_iNumKeyFrames = {};
    vector<KEYFRAME>	m_KeyFrames;
    KEYFRAME nowFrame = {};

public:
    static CChannel* Create(ifstream& ifs);
    virtual void Free() override;
};

NS_END