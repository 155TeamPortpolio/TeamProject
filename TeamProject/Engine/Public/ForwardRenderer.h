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
    CRenderer* GetRenderer(RENDERER_TYPE eType);

public:
    HRESULT Render_Priority(class PriorityPass* pPriorityPass);
    HRESULT Render_StaticShadow(class StaticShadowPass* pShadowPass, _bool clear = false);
    HRESULT Render_SkinnedShadow(class SkinnedShadowPass* pShadowPass, _bool clear = false);
    HRESULT Render_StaticMesh(class StaticOpaquePass* pOpaquePass, class InstancePass* pInstancePass);
    HRESULT Render_SkinnedMesh(class SkinnedOpaquePass* pOpaquePass);
    HRESULT Render_LightAcc();
    HRESULT Render_RimLight();
    HRESULT Render_SSAO();
    HRESULT Render_OutLine();
    HRESULT Render_Vanish();
    HRESULT Render_MotionBlur();
    HRESULT Render_Blended(class BlendedPass* pBlendPass);
    HRESULT Render_NonLight(class NonLightPass* pNonLightPass);
    HRESULT Render_Combined();
    HRESULT Render_Bloom();

public:
    void Update(_float dt);
public:
    void SetRimLightMode(RIMLIGHT eMode);
    void Add_OutLineCommand(const OUTLINE_COMMAND& command);
    void Add_MotionBlurCommand(const MOTIONBLUR_COMMAND& command);

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;

private:
    class CStaticMeshRenderer* m_pStaticRenderer;
    class CSkinnedMeshRenderer* m_pSkinnedRenderer;

    _float m_fStaticUpdateTimer = 0.f;
    _float m_fStaticUpdateInterval = 0.05f;

public:
    static CForwardRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;
};

NS_END