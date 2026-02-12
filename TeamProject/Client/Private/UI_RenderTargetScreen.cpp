#include "pch.h"
#include "UI_RenderTargetScreen.h"

#include "GameInstance.h"
#include "RectModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "UIDirector.h"

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
    Ready_RTVDrawObjects();

    m_pTransform->Scale(_float3(m_vViewPortSize.x, m_vViewPortSize.y, 1.f));

	return S_OK;
}

void CUI_RenderTargetScreen::Update(_float dt)
{
    if (InputDevice()->Key_Tap('I'))
        UIDirector()->Show_Wipeout();
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
    Get_Component<CRectModel>()->Set_RenderType(RENDER_PASS_TYPE::NONLIGHT_OPAQUE);
}

HRESULT CUI_RenderTargetScreen::Ready_RTVDrawObjects()
{
    if (FAILED(Create_RTVDrawObject("Proto_GameObject_Wipeout", "wipeout")))
        return E_FAIL;

    if (FAILED(Create_RTVDrawObject("Proto_GameObject_Switch", "switchv")))
        return E_FAIL;

    return S_OK;
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

HRESULT CUI_RenderTargetScreen::Create_RTVDrawObject(const string& strPrototypeTag, const string& strInstanceName)
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, strPrototypeTag }).Build(strInstanceName);
    if (!pObj)
        return E_FAIL;

    UIManager()->Add_UIObject(pObj, LevelManager()->Get_NowLevelKey());
    UIDirector()->Register(pObj);

    return S_OK;
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