#pragma once
#include "Renderer.h"

NS_BEGIN(Engine)

class CForwardRenderer final :
    public CRenderer
{
private:
    CForwardRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CForwardRenderer();
    HRESULT Initialize(class CTarget_Manager* pTargetManager, class CPipeLine* pPipeLine);

public:
    HRESULT Render_Forward(class OpaquePass* pOpaquePass, class InstancePass* pInstancePass);
    HRESULT Render_LightAcc();

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;

private:
    class CTexture* m_pRampTexture;

public:
    static CForwardRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;
};

NS_END