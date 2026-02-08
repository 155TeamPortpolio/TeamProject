#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CPostProcessCommand abstract:
    public CBase
{
protected:
    virtual ~CPostProcessCommand() = default;

public:
    virtual _uint GetPriority() const { return m_iPriority; }
    virtual _bool IsEnabled() const { return m_bEnabled; }
    virtual const string& GetName() const { return m_strName; }

public:
    virtual void Update(_float dt) PURE;
    virtual void Execute(ID3D11DeviceContext* pContext,
        class CRenderTarget* pInput,
        class CRenderTarget* pOutput) PURE;

protected:
    _uint m_iPriority = 0;
    _bool m_bEnabled = true;
    string m_strName = "";

public:
    virtual void Free() PURE;
};

class CHDRBloomCommand :
    public CPostProcessCommand
{
private:
    CHDRBloomCommand();
    virtual ~CHDRBloomCommand() = default;

public:
    virtual void Update(_float dt) override;
    virtual void Execute(ID3D11DeviceContext* pContext,
        class CRenderTarget* pInput,
        class CRenderTarget* pOutput) override;

private:
    _float m_fIntensity = 1.0f;

public:
    static CHDRBloomCommand* Create();
    virtual void Free() override;
};

class CGlitchCommand :
    public CPostProcessCommand
{
private:
    CGlitchCommand();
    virtual ~CGlitchCommand() = default;

public:
    virtual void Update(_float dt) override;
    virtual void Execute(ID3D11DeviceContext* pContext,
        class CRenderTarget* pInput,
        class CRenderTarget* pOutput) override;

private:
    class CTexture* m_pNoiseTexture = nullptr;
    _float m_fAccTime = 0.0f;
    _float m_fDuration = 1.f;

public:
    static CGlitchCommand* Create();
    virtual void Free() override;
};


class CRadialBlurCommand :
    public CPostProcessCommand
{
private:
    CRadialBlurCommand();
    virtual ~CRadialBlurCommand() = default;

public:
    virtual void Update(_float dt) override;
    virtual void Execute(ID3D11DeviceContext* pContext,
        class CRenderTarget* pInput,
        class CRenderTarget* pOutput) override;

private:
    _float2 m_vCenter = _float2(0.5, 0.5);
    _float m_fDuration = 1.f;
    _float m_fIntensity = 1.f;
    _float m_fAccTime = 0.0f;

public:
    static CRadialBlurCommand* Create();
    virtual void Free() override;
};

class CFogCommand :
    public CPostProcessCommand
{
private:
    CFogCommand();
    virtual ~CFogCommand() = default;

public:
    virtual void Update(_float dt) override;
    virtual void Execute(ID3D11DeviceContext* pContext,
        class CRenderTarget* pInput,
        class CRenderTarget* pOutput) override;

private:
    FOG_DESC m_fogDesc;

public:
    static CFogCommand* Create();
    virtual void Free() override;
};

class CGuassianBlurCommand :
    public CPostProcessCommand
{
private:
    CGuassianBlurCommand();
    virtual ~CGuassianBlurCommand() = default;

public:
    virtual void Update(_float dt) override;
    virtual void Execute(ID3D11DeviceContext* pContext,
        class CRenderTarget* pInput,
        class CRenderTarget* pOutput) override;

private:
    _float      m_fDuration;
    _float      m_fIntensity;
    _float      m_fAccTime;

public:
    static CGuassianBlurCommand* Create();
    virtual void Free() override;
};

class CAddictiveColorCommand :
    public CPostProcessCommand
{
private:
    CAddictiveColorCommand();
    virtual ~CAddictiveColorCommand() = default;

public:
    virtual void Update(_float dt) override;
    virtual void Execute(ID3D11DeviceContext* pContext,
        class CRenderTarget* pInput,
        class CRenderTarget* pOutput) override;

private:
    _float4      m_vAddictiveColor;

public:
    static CAddictiveColorCommand* Create();
    virtual void Free() override;
};
NS_END