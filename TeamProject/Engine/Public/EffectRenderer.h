#pragma once
#include "Renderer.h"

NS_BEGIN(Engine)

class CEffectRenderer final:
    public CRenderer
{
private:
    CEffectRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEffectRenderer();
    HRESULT Initialize(class CTarget_Manager* pTargetManager, class CPipeLine* pPipeLine);

public:
    HRESULT Render_Effect(class EffectPass* pEffectPass, class ParticlePass* pParticlePass);
    HRESULT Render_CustomTarget();
    void Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command);

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;

private:
    vector<RENDER_CUSTOM_COMMAND> m_RenderCommands;

public:
    static CEffectRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;
};

NS_END