#include "pch.h"
#include "UI_PartyAvatar.h"

#include "GameInstance.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "MaterialData.h"
#include "Animator3D.h" 
#include "Renderer.h"

HRESULT CUI_PartyAvatar::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CSkeletalModel>();
    Add_Component<CMaterial>();
    Add_Component<CAnimator3D>();

    return S_OK;
}

HRESULT CUI_PartyAvatar::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    AVATAR_DESC* pDesc = static_cast<AVATAR_DESC*>(pArg);
    m_strTargetKey = pDesc->strRenderTargetKey;

    _float2 vViewPortSize = GameInstance()->Get_ClientSize();
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), vViewPortSize.x / vViewPortSize.y, 0.1f, 100.f));

    m_pTransform->Rotate(_float3(0.f, XMConvertToRadians(180.f), 0.f));

    return S_OK;
}

void CUI_PartyAvatar::Awake()
{
    auto pModel = Get_Component<CSkeletalModel>();
    auto pMaterial = Get_Component<CMaterial>(); 
    auto pAnimator = Get_Component<CAnimator3D>();

    pModel->Link_Model(G_GlobalLevelKey, "Miyabi.model");
    pModel->Hide_MehsByName("HairShadow");
    
    pMaterial->Link_Material(G_GlobalLevelKey, "Miyabi.mat");
    
    pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Miyabi.model");
    pAnimator->Link_MetaData(G_GlobalLevelKey, "Miyabi_Meta.json");
    pAnimator->Set_Animation("Avatar_Female_Size02_Unagi_Ani_UI_CharacterSelect_Loop_02").Loop(true).Apply();

    m_pTransform->Set_Pos(_float3(0.f, -1.24f, 0.7f));
}

void CUI_PartyAvatar::Priority_Update(_float dt)
{
}

void CUI_PartyAvatar::Update(_float dt)
{
    Get_Component<CAnimator3D>()->Update_Animation(dt);

    RENDER_CUSTOM_COMMAND command = {};
    command.TargetKey = m_strTargetKey;
    command.bClear = true;
    command.DrawCallback = [this](ID3D11DeviceContext* pContext) { Render_RT(pContext); };
    RenderSystem()->Add_RenderCommand(command, CUSTOMTARGET::UI);
}

void CUI_PartyAvatar::Late_Update(_float dt)
{
}

void CUI_PartyAvatar::Render_RT(ID3D11DeviceContext* pContext)
{
    auto pRenderSystem = RenderSystem()->GetRenderer(RENDERER_TYPE::SKINNED);
    auto pModel = Get_Component<CSkeletalModel>();
    auto pMaterial = Get_Component<CMaterial>();

    for (_int i = 0; i < pModel->Get_MeshCount(); ++i)
    {
        if (pModel->isDrawable(i) == false) 
            continue;

        _int iIndex = pModel->Get_MaterialIndex(i);
        auto pShader = pMaterial->Get_Shader(iIndex);
        auto pInstance = pMaterial->Get_MaterialInstance(iIndex);
        pInstance->Override_Pass(m_strPassConstant);
        ID3D11InputLayout* pLayout;
        pRenderSystem->Get_InputLayout(pModel, pShader, i, m_strPassConstant, &pLayout);
         
        vector<_float4x4> BoneMatrices = Get_Component<CAnimator3D>()->Get_BoneMatrices(i);
        pInstance->Set_Param("g_CommandBoneMatrices", { BoneMatrices.data(), "float4x4[]", static_cast<_uint>(sizeof(_float4x4) * BoneMatrices.size())});
        pInstance->Set_Param("g_worldMatrix", { Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "matrix", sizeof(_float4x4) });
        pInstance->Set_Param("g_viewMatrix", { &m_ViewMatrix, "matrix", sizeof(_float4x4) });
        pInstance->Set_Param("g_projMatrix", { &m_ProjMatrix, "matrix", sizeof(_float4x4) });

        pContext->IASetInputLayout(pLayout);
        pMaterial->Apply_Material(pContext, iIndex);
        pModel->Draw(pContext, i);
        pInstance->Reset_Pass();
    }
}

CGameObject* CUI_PartyAvatar::Create()
{
    CUI_PartyAvatar* pInstance = new CUI_PartyAvatar();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_PartyAvatar");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_PartyAvatar::Clone(INIT_DESC* pArg)
{
    CUI_PartyAvatar* pInstance = new CUI_PartyAvatar(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_PartyAvatar");
        Safe_Release(pInstance);
    }
    return pInstance;
}