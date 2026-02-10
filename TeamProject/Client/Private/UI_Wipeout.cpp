#include "pch.h"
#include "UI_Wipeout.h"

#include "GameInstance.h"
#include "RectModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

#include "UI_WipeoutRTV.h"

HRESULT CUI_Wipeout::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CRectModel>();
    Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CUI_Wipeout::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto pUI = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_WipeoutRTV" }).Build("wipeoutRTV");
    if (pUI)
        UIManager()->Add_UIObject(pUI, LevelManager()->Get_NowLevelKey());

    auto pMaterial = Get_Component<CMaterial>();
    auto pMtrInst = CMaterialInstance::Create_Handle("wipeout", "UI", GameInstance()->Get_Device());
    pMaterial->Insert_MaterialInstance(pMtrInst, &m_iMtrlInstIdx);
    auto pMaterialData = pMtrInst->Get_MaterialData();
    if (pMaterialData)
    {
        pMaterialData->Link_Shader(G_GlobalLevelKey, "VTX_NorTex.hlsl");
        //pMaterialData->Link_Texture(G_GlobalLevelKey, "ScratchCardRewardIcon04.png", TEXTURE_TYPE::DIFFUSE);
    }

    _uint                   iNumViewports = { 1 };
    D3D11_VIEWPORT			ViewPortDesc{};
    GameInstance()->Get_Context()->RSGetViewports(&iNumViewports, &ViewPortDesc);

    const _float fViewportSizeX = ViewPortDesc.Width;
    const _float fViewportSizeY = ViewPortDesc.Height;
    m_pTransform->Scale(_float3(fViewportSizeX, fViewportSizeY, 1.f));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(fViewportSizeX, fViewportSizeY, 0.f, 1.f));
    pMtrInst->Set_Param("g_WorldMatrix", { Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "float4x4", sizeof(_float4x4) });
    pMtrInst->Set_Param("g_ViewMatrix", { &m_ViewMatrix, "float4x4", sizeof(_float4x4) });
    pMtrInst->Set_Param("g_ProjMatrix", { &m_ProjMatrix, "float4x4", sizeof(_float4x4) });

    Get_Component<CRectModel>()->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);

    {
        SHADER_PARAM param = {};
        auto pSRV = RenderSystem()->Get_CustomTargetSRV("wipeout");
        param.pData = pSRV;
        param.typeName = "Texture2D";
        param.iSize = 0;
        pMtrInst->Set_Param("DiffuseTexture", param);
    }

	return S_OK;
}

void CUI_Wipeout::Update(_float dt)
{
}

CGameObject* CUI_Wipeout::Create()
{
    CUI_Wipeout* pInstance = new CUI_Wipeout();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Wipeout");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Wipeout::Clone(INIT_DESC* pArg)
{
    CUI_Wipeout* pInstance = new CUI_Wipeout(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Wipeout");
        Safe_Release(pInstance);
    }
    return pInstance;
}