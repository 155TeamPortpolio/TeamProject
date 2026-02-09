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
    _float GetIntensity() const {return  m_fIntensity; }
    CGlitchCommand* SetNoiseTexture(class CTexture* pTexture);
    CGlitchCommand* SetDuration(_float fDuration);

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    class CTexture* m_pNoiseTexture = nullptr;
    _float m_fIntensity;
    _float m_fAccTime = 0.0f;
    _float m_fDuration = 1.f;

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
    _float m_fEaseT = 0.f;
    _float m_fDuration = 1.f;
    _float m_fIntensity = 1.f;
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
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

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
    _float GetIntensity() const { return m_fIntensity; }
    CGuassianBlurCommand* SetDuration(_float fDuration);
    CGuassianBlurCommand* SetIntensity(_float fIntensity);

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    _float      m_fDuration;
    _float      m_fIntensity;
    _float      m_fAccTime;

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
    CAddictiveColorCommand* SetAddictiveColor(_float3* vColor);
    virtual CPostProcessCommand* SetEnable(_bool bEnable) override;

public:
    virtual void Update(_float dt) override;
    virtual void Execute(class CPostRenderer* pRenderer) override;

private:
    _float3*      m_vAddictiveColor = nullptr;

public:
    static CAddictiveColorCommand* Create();
    virtual void Free() override;
};
NS_END