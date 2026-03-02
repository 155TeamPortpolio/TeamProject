#include "pch.h"
#include "GachaWeapon.h"

#include "StaticModel.h"
#include "Material.h"
#include "Child.h"
#include "GachaStage.h"

#include "Helper_Func.h"

CGachaWeapon::CGachaWeapon()
    :CGachaResult()
{
}

CGachaWeapon::CGachaWeapon(const CGachaWeapon& rhs)
    :CGachaResult(rhs)
{
}

void CGachaWeapon::SetResult(GACHA_RESULT_DESC Desc)
{
    auto pModel = Get_Component<CStaticModel>();
    auto pMaterial = Get_Component<CMaterial>(); 

    pModel->Link_Model("Gacha_Level", Desc.strModel);
    pMaterial->Link_Material("Gacha_Level", Desc.strMaterial);

    _vector4 vRot = _vector4(Desc.RotX, Desc.RotY, Desc.RotZ, Desc.RotW);
    m_pTransform->Set_Quaternion(vRot);

    m_vInitRot = vRot;
    m_fRotElapsedTime = 0.f;

    m_bRevealEffect = false;
}

HRESULT CGachaWeapon::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();

    return S_OK;
}

HRESULT CGachaWeapon::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGachaWeapon::Awake()
{
}

void CGachaWeapon::Priority_Update(_float dt)
{
}

void CGachaWeapon::Update(_float dt)
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
    else if (m_bRevealEffect == false)
    {
        auto pParent = dynamic_cast<CGachaStage*>(Get_Component<CChild>()->Get_Parent());
        pParent->PlayRevealEffect();

        m_bRevealEffect = true;
    }
}

void CGachaWeapon::Late_Update(_float dt)
{
}

CGachaWeapon* CGachaWeapon::Create()
{
    CGachaWeapon* Instance = new CGachaWeapon();
    if (FAILED(Instance->Initialize_Prototype()))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

CGameObject* CGachaWeapon::Clone(INIT_DESC* pArg)
{
    CGachaWeapon* Instance = new CGachaWeapon(*this);
    if (FAILED(Instance->Initialize(pArg)))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

void CGachaWeapon::Free()
{
    __super::Free();
}
