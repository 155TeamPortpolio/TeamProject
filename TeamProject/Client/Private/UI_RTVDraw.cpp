#include "pch.h"
#include "UI_RTVDraw.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 
#include "Sprite2D.h"
#include "Shader.h"
#include "Renderer.h"

HRESULT CUI_RTVDraw::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_RTVDraw::Initialize(INIT_DESC* pArg)
{
    RTVDRAW_DESC* pDesc = static_cast<RTVDRAW_DESC*>(pArg);
    m_hRenderTargetScreen = pDesc->hRenderTargetScreen;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // 뷰, 프로젝션 행렬 구성
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_WinSize.x, m_WinSize.y, 0.f, 1.f));

    return S_OK;
}

void CUI_RTVDraw::Awake()
{
    // 자식 레이어 커스텀으로
    for (auto& pChild : Get_Component<CObjectContainer>()->Get_Children())
        pChild->SetRenderLayer(RENDER_LAYER::CustomOnly);
}

void CUI_RTVDraw::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_RTVDraw::Update_RTV(const string& strTargetKey, _bool isClear)
{
    RENDER_CUSTOM_COMMAND command = {};
    command.TargetKey = strTargetKey;
    command.bClear = isClear;
    command.DrawCallback = [this](ID3D11DeviceContext* pContext) { Render_RT(pContext); };
    RenderSystem()->Add_RenderCommand(command, CUSTOMTARGET::UI);
}

void CUI_RTVDraw::Set_RenderTargetScreenRenderLayer(RENDER_LAYER eLayer)
{
    if (m_hRenderTargetScreen.isAlive())
        m_hRenderTargetScreen.Get()->SetRenderLayer(eLayer);
}

void CUI_RTVDraw::Render_RT(ID3D11DeviceContext* pContext)
{
    for (auto& pChild : Get_Component<CObjectContainer>()->Get_Children())
        Render_RTRecursive(pChild, pContext);
}

void CUI_RTVDraw::Render_RTRecursive(CGameObject* pObj, ID3D11DeviceContext* pContext)
{
    if (!pObj)
        return;

    if (!pObj->Is_Alive())
        return;

    if (auto pSprite = pObj->Get_Component<CSprite2D>())
    {
        if (auto pShader = pSprite->Get_Shader())
        {
            ID3D11InputLayout* pLayout = { nullptr };
            const string strPassConstant = pSprite->Get_PassConstant();
            string strCustomPassConstant = "Opaque_Custom";

            if (strPassConstant == "UI_StencilWrite")
                strCustomPassConstant = "StencilWrite_Custom";
            else if (strPassConstant == "Opaque_StencilTest")
                strCustomPassConstant = "Opaque_StencilTest_Custom";
            else if (strPassConstant == "UVAnimation_StencilTest")
                strCustomPassConstant = "UVAnimation_StencilTest_Custom";
            else if (strPassConstant == "SoftDirectionalOutline")
                strCustomPassConstant = "SoftDirectionalOutline_Custom";

            RenderSystem()->GetRenderer(RENDERER_TYPE::UI)->Get_BufferInputLayout(pSprite->Get_Buffer(), pShader, strCustomPassConstant, &pLayout);
            pContext->IASetInputLayout(pLayout);
            pSprite->Set_Param("g_WorldMatrix", { pObj->Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "matrix", sizeof(_float4x4) });
            pSprite->Set_Param("g_ViewMatrix", { &m_ViewMatrix, "matrix", sizeof(_float4x4) });
            pSprite->Set_Param("g_ProjMatrix", { &m_ProjMatrix, "matrix", sizeof(_float4x4) });
            pSprite->Apply_Shader(pContext);
            pSprite->Set_Param("vColor", { dynamic_cast<CUI_Object*>(pObj)->Get_LinearColorPtr(), "float4", sizeof(_float4)});

            pShader->Apply(strCustomPassConstant, pContext);
            pSprite->Draw_Sprite(pContext);
        }
    }

    if (auto pContainer = pObj->Get_Component<CObjectContainer>())
    {
        auto& children = pContainer->Get_Children();
        for (auto& pChild : children)
        {
            Render_RTRecursive(pChild, pContext);
        }
    }
}