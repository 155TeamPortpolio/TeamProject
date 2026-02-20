#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPostProcessCommand abstract:
    public CBase
{
public:
    enum class EFFECT_TYPE{ REPLACE, COMBINE  };

protected:
    virtual ~CPostProcessCommand() = default;

public:
    virtual _uint GetPriority() const { return m_iPriority; }
    virtual _bool IsEnabled() const { return m_bEnabled; }
    virtual const string& GetName() const { return m_strName; }
    virtual EFFECT_TYPE GetEffectType() const { return m_eEffectType; };
    virtual const string& GetOutPutTargetName() const { return m_strOutputTargetName; }
    virtual CPostProcessCommand* SetEnable(_bool bEnable);

public:
    virtual void Update(_float dt) PURE;
    virtual void Execute(class CPostRenderer* pRenderer) PURE;

protected:
    EFFECT_TYPE m_eEffectType;
    _bool m_bEnabled = true;
    _uint m_iPriority = 0;
    string m_strName = "";
    string m_strOutputTargetName = "";

public:
    virtual void Free() PURE;
};

class ENGINE_DLL CHDRBloomCommand :
    public CPostProcessCommand
{
private:
    CHDRBloomCommand();
    virtual ~CHDRBloomCommand() = default;

public:
    _float GetIntensity() const { return m_fIntensity; }
    CHDRBloomCommand* SetIntensity(_float fIntensity);

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    _float m_fIntensity = 1.0f;

public:
    static CHDRBloomCommand* Create();
    virtual void Free() override;
};

class ENGINE_DLL CGlitchCommand :
    public CPostProcessCommand
{
private:
    CGlitchCommand();
    virtual ~CGlitchCommand() = default;

public:
    ID3D11ShaderResourceView* GetNoiseSRV();
    _float GetIntensity() const {return  m_fIntensity * m_fEaseT; }
    CGlitchCommand* SetNoiseTexture(class CTexture* pTexture);
    CGlitchCommand* SetIntensity(_float fIntensity);
    CGlitchCommand* SetDuration(_float fDuration);
    CGlitchCommand* SetEaseType(EaseType easeType);

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    class CTexture* m_pNoiseTexture = nullptr;
    _float m_fIntensity = 1.f;
    _float m_fAccTime = 0.0f;
    _float m_fDuration = 1.f;
    EaseType m_EaseType = EaseType::InOutSine;
    _float m_fEaseT = 0.f;

public:
    static CGlitchCommand* Create();
    virtual void Free() override;
};

class ENGINE_DLL CRadialBlurCommand :
    public CPostProcessCommand
{
private:
    CRadialBlurCommand();
    virtual ~CRadialBlurCommand() = default;

public:
    _float2 GetCenter() const { return m_vCenter; }
    _float GetIntensity() const { return m_fIntensity * m_fEaseT; }
    CRadialBlurCommand* SetCenter(_float2 vCenter);
    CRadialBlurCommand* SetDuration(_float fDuration);
    CRadialBlurCommand* SetIntensity(_float fIntensity);
    CRadialBlurCommand* SetEaseType(EaseType easeType);

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    _float2 m_vCenter = _float2(0.5, 0.5);
    _float m_fDuration = 1.f;
    _float m_fIntensity = 1.f;
    _float m_fEaseT = 0.f;
    EaseType m_EaseType = EaseType::InOutSine;
    _float m_fAccTime = 0.0f;

public:
    static CRadialBlurCommand* Create();
    virtual void Free() override;
};

class ENGINE_DLL CFogCommand :
    public CPostProcessCommand
{
private:
    CFogCommand();
    virtual ~CFogCommand() = default;
    
public:
    FOG_DESC GetFogDesc() const { return m_FogDesc; }
    CFogCommand* SetFogDesc(FOG_DESC Desc);
    CFogCommand* SetColor(_float4 vColor);
    CFogCommand* SetDensity(_float fDensity);

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    FOG_DESC    m_FogDesc;

public:
    static CFogCommand* Create();
    virtual void Free() override;
};

class ENGINE_DLL CGuassianBlurCommand :
    public CPostProcessCommand
{
private:
    CGuassianBlurCommand();
    virtual ~CGuassianBlurCommand() = default;

public:
    _float GetIntensity() const { return m_fIntensity * m_fEaseT; }
    CGuassianBlurCommand* SetDuration(_float fDuration);
    CGuassianBlurCommand* SetIntensity(_float fIntensity);
    CGuassianBlurCommand* SetEaseType(EaseType easeType);

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    _float      m_fDuration = 0.f;
    _float      m_fIntensity = 1.f;
    _float      m_fAccTime = 0.f;
    _float      m_fEaseT = 0.f;
    EaseType    m_EaseType = EaseType::InOutSine;

public:
    static CGuassianBlurCommand* Create();
    virtual void Free() override;
};

class ENGINE_DLL CAddictiveColorCommand :
    public CPostProcessCommand
{
private:
    CAddictiveColorCommand();
    virtual ~CAddictiveColorCommand() = default;

public:
    _float3* GetAddictiveColor() { return m_vAddictiveColor; }
    _bool GetSkinned() const { return m_bSkinned; }
    CAddictiveColorCommand* SetAddictiveColor(_float3* vColor);
    CPostProcessCommand* SetSkinned(_bool bSkinned);
    virtual CPostProcessCommand* SetEnable(_bool bEnable) override;

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    _float3*      m_vAddictiveColor = nullptr;
    _bool         m_bSkinned = false;

public:
    static CAddictiveColorCommand* Create();
    virtual void Free() override;
};

class ENGINE_DLL CSaturationCommand :
    public CPostProcessCommand
{
private:
    CSaturationCommand();
    virtual ~CSaturationCommand() = default;

public:
    _uint  GetSaturationType() const { return m_SaturationType; }
    _float GetIntensity() const { return m_fIntensity * m_fEaseT; }
    CSaturationCommand* SetDuration(_float fDuration);
    CSaturationCommand* SetIntensity(_float fIntensity);
    CSaturationCommand* SetEaseType(EaseType easeType);
    CSaturationCommand* SetSaturationType(_uint flags);

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    _float      m_fDuration = 0.f;
    _float      m_fIntensity = 1.f;
    _float      m_fAccTime = 0.f;
    _float      m_fEaseT = 0.f;
    EaseType    m_EaseType = EaseType::InOutSine;

    _uint       m_SaturationType = static_cast<_uint>(SATURATIONTYPE::STATIC);
public:
    static CSaturationCommand* Create();
    virtual void Free() override;
};

class ENGINE_DLL CDistortionCommand :
    public CPostProcessCommand
{
private:
    CDistortionCommand();
    virtual ~CDistortionCommand() = default;

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

public:
    static CDistortionCommand* Create();
    virtual void Free() override;
};

class ENGINE_DLL CFlareCommand :
    public CPostProcessCommand
{
private:
    CFlareCommand();    
    virtual ~CFlareCommand() = default;

public:
    _float GetIntensity() const { return m_fIntensity; }
    _float2 GetCenter() const { return m_vCenter; }
    _float3 GetColor() const { return m_vColor; }
    _float GetAccTime() const { return m_fAccTime; }
    CFlareCommand* SetDuration(_float fDuration);
    CFlareCommand* SetIntensity(_float fIntensity);
    CFlareCommand* SetColor(_float3 vColor);
    CFlareCommand* SetCenter(_float2 vCenter = _float2(0.5, 0.5f));

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    _float      m_fDuration = 0.f;
    _float      m_fAccTime = 0.f;

    _float      m_fIntensity = 1.f;
    _float3     m_vColor = _float3(1.f, 1.f, 1.f);
    _float2     m_vCenter = _float2(0.5, 0.5f);

public:
    static CFlareCommand* Create();
    virtual void Free() override;
};


class ENGINE_DLL CNoiseCommand :
    public CPostProcessCommand
{
private:
    CNoiseCommand();
    virtual ~CNoiseCommand() = default;

public:
    CTexture* GetTexture() const { return m_Textures[m_iCurrentIdx]; }
    CNoiseCommand* SetDuration(_float fDuration);
    CNoiseCommand* SetNum(_int iNum);
    CNoiseCommand* SetTextures(vector<class CTexture*> Textures);

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    vector<class CTexture*> m_Textures;
    _float      m_fDuration = 0.f;
    _float      m_fAccTime = 0.f;
    _int        m_iCount = 0;
    _int        m_iCurrentIdx = 0;

public:
    static CNoiseCommand* Create();
    virtual void Free() override;
};
NS_END