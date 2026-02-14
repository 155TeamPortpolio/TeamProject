#include "pch.h"
#include "UI_AvatarTest.h"

#include "GameInstance.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "MaterialData.h"
#include "Animator3D.h" 
#include "Renderer.h"

HRESULT CUI_AvatarTest::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CSkeletalModel>();
    Add_Component<CMaterial>();
    Add_Component<CAnimator3D>();

    return S_OK;
}

HRESULT CUI_AvatarTest::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _float2 vViewPortSize = GameInstance()->Get_ClientSize();
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), vViewPortSize.x / vViewPortSize.y, 0.1f, 100.f));// XMMatrixOrthographicLH(vViewPortSize.x, vViewPortSize.y, 0.f, 1.f));

    m_pTransform->Set_Pos(_float4(0.f, -1.3f, 0.6f, 1.f ));
    m_pTransform->Rotate(_float3(0.f, XMConvertToRadians(180.f), 0.f));

    return S_OK;
}

void CUI_AvatarTest::Awake()
{
    auto pModel = Get_Component<CSkeletalModel>();
    auto pMaterial = Get_Component<CMaterial>(); 
    auto pAnimator = Get_Component<CAnimator3D>();

    pModel->Link_Model(G_GlobalLevelKey, "Miyabi.model");
    pModel->Hide_MehsByName("HairShadow");

    pMaterial->Link_Material(G_GlobalLevelKey, "Miyabi.mat");

    pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Miyabi.model");
    pAnimator->Link_MetaData(G_GlobalLevelKey, "Miyabi_Meta.json");
    pAnimator->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Gacha_Loop").Loop(true).Apply();

    auto& materialInstances = pMaterial->Get_MaterialInstances();
    for (auto& pInstance : materialInstances)
    {
        pInstance->Override_Pass(m_strPassConstant);
        pInstance->Set_Param("g_worldMatrix", { Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "matrix", sizeof(_float4x4) });
        pInstance->Set_Param("g_viewMatrix", { &m_ViewMatrix, "matrix", sizeof(_float4x4) });
        pInstance->Set_Param("g_projMatrix", { &m_ProjMatrix, "matrix", sizeof(_float4x4) });
    }
}

void CUI_AvatarTest::Priority_Update(_float dt)
{
}

void CUI_AvatarTest::Update(_float dt)
{
    Get_Component<CAnimator3D>()->Update_Animation(dt);

    RENDER_CUSTOM_COMMAND command = {};
    command.TargetKey = m_strTargetKey;
    command.bClear = true;
    command.DrawCallback = [this](ID3D11DeviceContext* pContext) { Render_RT(pContext); };
    RenderSystem()->Add_RenderCommand(command, CUSTOMTARGET::UI);
}

void CUI_AvatarTest::Late_Update(_float dt)
{
}

void CUI_AvatarTest::Render_RT(ID3D11DeviceContext* pContext)
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

        pContext->IASetInputLayout(pLayout);
        pMaterial->Apply_Material(pContext, iIndex);
        pModel->Draw(pContext, i);
        pInstance->Reset_Pass();
    }
}

CGameObject* CUI_AvatarTest::Create()
{
    CUI_AvatarTest* pInstance = new CUI_AvatarTest();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_AvatarTest");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_AvatarTest::Clone(INIT_DESC* pArg)
{
    CUI_AvatarTest* pInstance = new CUI_AvatarTest(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_AvatarTest");
        Safe_Release(pInstance);
    }
    return pInstance;
}