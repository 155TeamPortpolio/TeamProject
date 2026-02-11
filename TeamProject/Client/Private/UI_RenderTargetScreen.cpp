#include "pch.h"
#include "UI_RenderTargetScreen.h"

#include "GameInstance.h"
#include "RectModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

HRESULT CUI_RenderTargetScreen::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CRectModel>();
    Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CUI_RenderTargetScreen::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_vViewPortSize = GameInstance()->Get_ClientSize();

    if (FAILED(Ready_Components()))
        return E_FAIL;

    Ready_RTV();
    Ready_ViewProj();
    Ready_RenderState();
    Ready_RTV_DrawObjects();

    m_pTransform->Scale(_float3(m_vViewPortSize.x, m_vViewPortSize.y, 1.f));

	return S_OK;
}

void CUI_RenderTargetScreen::Update(_float dt)
{
}

HRESULT CUI_RenderTargetScreen::Ready_Components()
{
    auto pMaterial = Get_Component<CMaterial>();
    auto pMtrlInst = CMaterialInstance::Create_Handle("renderTargetScreen", "UI", GameInstance()->Get_Device());

    pMaterial->Insert_MaterialInstance(pMtrlInst, &m_iMtrlInstIdx);

    auto pMaterialData = pMtrlInst->Get_MaterialData();
    if (pMaterialData)
        pMaterialData->Link_Shader(G_GlobalLevelKey, "VTX_NorTex.hlsl");

    pMtrlInst->Set_Param("g_WorldMatrix", { Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "float4x4", sizeof(_float4x4) });
    pMtrlInst->Set_Param("g_ViewMatrix", { &m_ViewMatrix, "float4x4", sizeof(_float4x4) });
    pMtrlInst->Set_Param("g_ProjMatrix", { &m_ProjMatrix, "float4x4", sizeof(_float4x4) });

    return S_OK;
}

void CUI_RenderTargetScreen::Ready_RTV()
{
    Create_RTV();
    Bind_RTV();
}

void CUI_RenderTargetScreen::Ready_ViewProj()
{
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_vViewPortSize.x, m_vViewPortSize.y, 0.f, 1.f));
}

void CUI_RenderTargetScreen::Ready_RenderState()
{
    Get_Component<CRectModel>()->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);
}

void CUI_RenderTargetScreen::Ready_RTV_DrawObjects()
{
    auto pWipeout = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_Wipeout" }).Build("wipeout");
    if (pWipeout)
        UIManager()->Add_UIObject(pWipeout, LevelManager()->Get_NowLevelKey());

    auto pSwitch = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_Switch" }).Build("switch");
    if (pSwitch)
        UIManager()->Add_UIObject(pSwitch, LevelManager()->Get_NowLevelKey());
}

void CUI_RenderTargetScreen::Create_RTV()
{
    RenderTargetDesc desc = {};
    desc.Key = strRTVTag;
    desc.Width = m_vViewPortSize.x;
    desc.Height = m_vViewPortSize.y;
    RenderSystem()->Create_RenderTarget(desc);
}

void CUI_RenderTargetScreen::Bind_RTV()
{
    auto pMtrlInst = Get_Component<CMaterial>()->Get_MaterialInstance(m_iMtrlInstIdx);

    {
        SHADER_PARAM param = {};
        auto pSRV = RenderSystem()->Get_CustomTargetSRV(strRTVTag);
        param.pData = pSRV;
        param.typeName = "Texture2D";
        param.iSize = 0;
        pMtrlInst->Set_Param("DiffuseTexture", param);
    }

    {
        SHADER_PARAM param = {};
        auto pSRV = RenderSystem()->Get_EngineTargetSRV("Target_Skinned_Depth");
        param.pData = pSRV;
        param.typeName = "Texture2D";
        param.iSize = 0;
        pMtrlInst->Set_Param("MaskTexture", param);
    }
}

CGameObject* CUI_RenderTargetScreen::Create()
{
    CUI_RenderTargetScreen* pInstance = new CUI_RenderTargetScreen();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_RenderTargetScreen");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_RenderTargetScreen::Clone(INIT_DESC* pArg)
{
    CUI_RenderTargetScreen* pInstance = new CUI_RenderTargetScreen(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_RenderTargetScreen");
        Safe_Release(pInstance);
    }
    return pInstance;
}