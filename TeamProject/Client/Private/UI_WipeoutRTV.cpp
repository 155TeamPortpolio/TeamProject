#include "pch.h"
#include "UI_WipeoutRTV.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 
#include "Sprite2D.h"
#include "Shader.h"
#include "Renderer.h"

HRESULT CUI_WipeoutRTV::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_WipeoutRTV::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("wipeout.json")));

    auto pSprite = Get_Component<CSprite2D>();

    // 뷰, 프로젝션 행렬 구성
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_WinSize.x, m_WinSize.y, 0.f, 1.f));

    // 자식 레이어 커스텀으로
    for (auto& pChild : Get_Component<CObjectContainer>()->Get_Children())
        pChild->SetRenderLayer(RENDER_LAYER::CustomOnly);

	return S_OK;
}

void CUI_WipeoutRTV::Awake()
{
}

void CUI_WipeoutRTV::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    RENDER_CUSTOM_COMMAND command = {};
    command.TargetKey = "renderTargetScreen";
    command.bClear = true;
    command.DrawCallback = [this](ID3D11DeviceContext* pContext) { Render_RT(pContext); };
    RenderSystem()->Add_RenderCommand(command, CUSTOMTARGET::UI);
}

void CUI_WipeoutRTV::Render_RT(ID3D11DeviceContext* pContext)
{
    for (auto& pChild : Get_Component<CObjectContainer>()->Get_Children())
        Render_RTRecursive(pChild, pContext);
}

void CUI_WipeoutRTV::Render_RTRecursive(CGameObject* pObj, ID3D11DeviceContext* pContext)
{
    if (!pObj)
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

            RenderSystem()->GetRenderer(RENDERER_TYPE::UI)->Get_BufferInputLayout(pSprite->Get_Buffer(), pShader, strCustomPassConstant, &pLayout);
            pContext->IASetInputLayout(pLayout);
            pSprite->Set_Param("g_WorldMatrix", { pObj->Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "matrix", sizeof(_float4x4) });
            pSprite->Set_Param("g_ViewMatrix", { &m_ViewMatrix, "matrix", sizeof(_float4x4) });
            pSprite->Set_Param("g_ProjMatrix", { &m_ProjMatrix, "matrix", sizeof(_float4x4) });
            pSprite->Apply_Shader(pContext);
            pSprite->Set_Param("vColor", { &m_vColor, "float4", sizeof(_float4) });

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

CGameObject* CUI_WipeoutRTV::Create()
{
    CUI_WipeoutRTV* pInstance = new CUI_WipeoutRTV();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_WipeoutRTV");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_WipeoutRTV::Clone(INIT_DESC* pArg)
{
    CUI_WipeoutRTV* pInstance = new CUI_WipeoutRTV(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_WipeoutRTV");
        Safe_Release(pInstance);
    }
    return pInstance;
}