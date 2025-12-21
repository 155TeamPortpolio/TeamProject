#pragma once
#include "Renderer.h"

NS_BEGIN(Engine)

class CUIRenderer final :
    public CRenderer
{
private:
    CUIRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CUIRenderer();
    HRESULT Initialize(class CTarget_Manager* pTargetManager, class CPipeLine* pPipeLine);

public:
    HRESULT Render_3D(class UI3DPass* pUI3DPass);
    HRESULT Render_2D(class UIPass* pUIPass);
    HRESULT Render_CustomTarget();
    void Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command);

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;

private:
    vector<RENDER_CUSTOM_COMMAND> m_RenderCommands;

public:
    static CUIRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;
};

NS_END