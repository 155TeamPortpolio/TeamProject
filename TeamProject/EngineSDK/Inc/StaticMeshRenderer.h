#pragma once
#include "Renderer.h"

NS_BEGIN(Engine)

class CStaticMeshRenderer final :
    public CRenderer
{
private:
    CStaticMeshRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CStaticMeshRenderer();
    HRESULT Initialize(class CTarget_Manager* pTargetManager, class CPipeLine* pPipeLine);

public:
    HRESULT Render_StaticMesh(class StaticOpaquePass* pOpaquePass, class InstancePass* pInstancePass);
    HRESULT Render_StaticMesh_Bloom();
    HRESULT Render_SSAO();
    HRESULT Render_StaticMesh_LightAcc();
    HRESULT Render_StaticMesh_Combined();

public:
    void Update(_float dt);

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;

private:
    HRESULT CreateSSAONoiseTexture();

private:
    ID3D11ShaderResourceView* m_pSSAONoiseTexture = { nullptr };

public:
    static CStaticMeshRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;
};

NS_END