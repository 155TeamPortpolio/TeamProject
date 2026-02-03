#pragma once
#include "CamObject.h"

NS_BEGIN(Client)

class CShadowCam final: public CCamObject
{
private:
    typedef struct ShadowLightInfo {
        _vector4 Position;
        _float4 Diffuse;
        _float4 Ambient;
        _float4 Specular;
        _float Intensity;
    }SHADOWLIGHT_DESC;

private:
    CShadowCam() {}
    CShadowCam(const CShadowCam& rhs) :CCamObject(rhs) {}
    virtual ~CShadowCam() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Update(_float dt)           override; 

private:
    void Update_LightByTime(DAYPHASE_DESC Desc);
    void Update_LightTransition(_float dt);

private:
    _bool m_bIsTransition = false;
    _float m_fTransitionTime = 0.f;
    _float m_fTransitionDuration = 2.0f; 
    SHADOWLIGHT_DESC m_StartLight, m_TargetLight;

public:
    static  CShadowCam* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END