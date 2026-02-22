#include "pch.h"
#include "UltimateBG.h"

#include "GameInstance.h"
#include "RectModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

void CUltimateBG::Show_Ultimate(CHARACTER eCharacter, _float duration)
{
    auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);

    switch (eCharacter)
    {
    case CHARACTER::Miyabi:
        pMaterialInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
        break;
    case CHARACTER::JaneDoe:
        pMaterialInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 1);
        break;
    case CHARACTER::Corin:
        pMaterialInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 2);
        break;
    }
    m_fAccTime = 0.f;
    m_fDuration = duration;
    SetRenderLayer(RENDER_LAYER::Default);
}

HRESULT CUltimateBG::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CRectModel>();
    Add_Component<CMaterial>();

    return S_OK;
}

HRESULT CUltimateBG::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_vViewPortSize = GameInstance()->Get_ClientSize();

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_vViewPortSize.x, m_vViewPortSize.y, 0.f, 1.f));
    m_pTransform->Scale(_float3(m_vViewPortSize.x, m_vViewPortSize.y, 1.f));

    Get_Component<CRectModel>()->Set_RenderType(RENDER_PASS_TYPE::NONLIGHT_OPAQUE);
    SetRenderLayer(RENDER_LAYER::None);

    return S_OK;
}

void CUltimateBG::Awake()
{
    auto pMaterial = Get_Component<CMaterial>();
    auto pMaterialInstance = CMaterialInstance::Create_Handle("Ultimate", "UI", GameInstance()->Get_Device());

    pMaterial->Insert_MaterialInstance(pMaterialInstance, 0);

    auto pMaterialData = pMaterialInstance->Get_MaterialData();
    if (pMaterialData)
    {
        pMaterialData->Link_Shader(G_GlobalLevelKey, "VTX_NorTex.hlsl");
        pMaterialData->Link_Texture(G_GlobalLevelKey, "UltimateBg_Miyabi.png", TEXTURE_TYPE::DIFFUSE);
        pMaterialData->Link_Texture(G_GlobalLevelKey, "UltimateBg_JaneDoe.png", TEXTURE_TYPE::DIFFUSE);
        pMaterialData->Link_Texture(G_GlobalLevelKey, "UltimateBg_Corin.png", TEXTURE_TYPE::DIFFUSE);
    }
    pMaterialInstance->Set_Param("g_WorldMatrix", { Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "float4x4", sizeof(_float4x4) });
    pMaterialInstance->Set_Param("g_ViewMatrix", { &m_ViewMatrix, "float4x4", sizeof(_float4x4) });
    pMaterialInstance->Set_Param("g_ProjMatrix", { &m_ProjMatrix, "float4x4", sizeof(_float4x4) });

    Ready_RT();
}

void CUltimateBG::Update(_float dt)
{
    m_fAccTime += dt;
    if (m_fDuration <= m_fAccTime)
    {
        SetRenderLayer(RENDER_LAYER::None);
    }
}

void CUltimateBG::Ready_RT()
{
    auto pMtrlInst = Get_Component<CMaterial>()->Get_MaterialInstance(0);

    {
        SHADER_PARAM param = {};
        auto pSRV = RenderSystem()->Get_EngineTargetSRV("Target_Skinned_Depth");
        param.pData = pSRV;
        param.typeName = "Texture2D";
        param.iSize = 0;
        pMtrlInst->Set_Param("MaskTexture", param);
    }
}

CGameObject* CUltimateBG::Create()
{
    CUltimateBG* pInstance = new CUltimateBG();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUltimateBG");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUltimateBG::Clone(INIT_DESC* pArg)
{
    CUltimateBG* pInstance = new CUltimateBG(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUltimateBG");
        Safe_Release(pInstance);
    }
    return pInstance;
}
