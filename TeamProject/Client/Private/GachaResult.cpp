#include "pch.h"
#include "GachaResult.h"

#include "StaticModel.h"
#include "Material.h"

#include "Helper_Func.h"

CGachaResult::CGachaResult()
    :CGameObject()
{
}

CGachaResult::CGachaResult(const CGachaResult& rhs)
    :CGameObject(rhs)
{
}

void CGachaResult::SetResult(string strModel, string strMaterial, _float4 vRot)
{
    auto pModel = Get_Component<CStaticModel>();
    auto pMaterial = Get_Component<CMaterial>();

    pModel->Link_Model("Gacha_Level", strModel);
    pMaterial->Link_Material("Gacha_Level", strMaterial);

    m_pTransform->Set_Quaternion(_vector4(vRot));

    m_vInitRot = vRot;
    m_fRotElapsedTime = 0.f;
}

HRESULT CGachaResult::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();

    pModel->Link_Model("Gacha_Level", "Weapon_A_Common_03out.model");
    pMaterial->Link_Material("Gacha_Level", "Weapon_A_Common_03out.mat");

    return S_OK;
}

HRESULT CGachaResult::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGachaResult::Awake()
{
}

void CGachaResult::Priority_Update(_float dt)
{
}

void CGachaResult::Update(_float dt)
{
    if (m_fRotElapsedTime < m_fRotDuration)
    {
        m_fRotElapsedTime += dt;

        _float fProgress = min(m_fRotElapsedTime / m_fRotDuration, 1.f);

        _float fEasedProgress = Math::ApplyEase(EaseType::OutCubic, fProgress);

        _float fYRotation = fEasedProgress * XM_2PI;

        XMVECTOR vInitQuat = m_vInitRot;
        XMVECTOR vYRotQuat = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), fYRotation);
        XMVECTOR vFinalQuat = XMQuaternionMultiply(vInitQuat, vYRotQuat);

        m_pTransform->Set_Quaternion(vFinalQuat);
    }
}

void CGachaResult::Late_Update(_float dt)
{
}

CGachaResult* CGachaResult::Create()
{
    CGachaResult* Instance = new CGachaResult();
    if (FAILED(Instance->Initialize_Prototype()))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

CGameObject* CGachaResult::Clone(INIT_DESC* pArg)
{
    CGachaResult* Instance = new CGachaResult(*this);
    if (FAILED(Instance->Initialize(pArg)))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

void CGachaResult::Free()
{
    __super::Free();
}
