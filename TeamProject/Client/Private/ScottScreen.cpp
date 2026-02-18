#include "pch.h"
#include "ScottScreen.h"
#include "GameInstance.h"

#include "Animator3D.h"
#include "StaticModel.h"
#include "ModelData.h"
#include "Material.h"
#include "AudioSource.h"

CScottScreen::CScottScreen()
    : CAmbientActor()
{
}

CScottScreen::CScottScreen(const CScottScreen& rhs)
    : CAmbientActor(rhs)
{
}

HRESULT CScottScreen::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CStaticModel>();
    Add_Component<CMaterial>();
    Add_Component<CAudioSource>();

    auto pModel = Get_Component<CStaticModel>();
    pModel->Link_Model("Scott_Level", "OnlyScreen.model");
    auto pMaterial = Get_Component<CMaterial>();
    pMaterial->Link_Material("Scott_Level", "OnlyScreen.mat");

    return S_OK;
}

HRESULT CScottScreen::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    AMBIENTACTOR_DESC* pDesc = static_cast<AMBIENTACTOR_DESC*>(pArg);

    m_pScreen[0] = { 8, 6, 45};
    m_pScreen[1] = { 8, 9, 72 };
    m_pScreen[2] = { 10, 5, 48 };
    m_pScreen[3] = { 7, 6, 41 };
    m_pScreen[4] = { 4, 4, 16 };
    m_pScreen[5] = { 8, 9, 72 };

    return S_OK;
}

void CScottScreen::Awake()
{
    auto pMaterial = Get_Component<CMaterial>();
    auto pMaterialInstances = pMaterial->Get_MaterialInstances();
    if (pMaterialInstances.size() < m_iScreenCount)
        return;

    for (_int idx = 0; idx < m_iScreenCount; ++idx)
    {
        pMaterialInstances[idx]->Set_Param("FrameIndex", { &m_pScreen[idx].iCurrentFrameIndex, "int", sizeof(_int) });
        pMaterialInstances[idx]->Set_Param("Col", { &m_pScreen[idx].iCol, "int", sizeof(_int) });
        pMaterialInstances[idx]->Set_Param("Row", { &m_pScreen[idx].iRow, "int", sizeof(_int) });

        m_pScreen[idx].iCurrentFrameIndex = Helper::Get_Random_Int(0, m_pScreen[idx].iMaxFrame - 1);
    }
}

void CScottScreen::Priority_Update(_float dt)
{
}

void CScottScreen::Update(_float dt)
{
    m_fFrameElapsedTime += dt;
    if (m_fFrameElapsedTime > m_fFrameDuration)
    {
        for (_int i = 0; i < m_iScreenCount; ++i)
        {
            ++m_pScreen[i].iCurrentFrameIndex;

            if (m_pScreen[i].iCurrentFrameIndex >= m_pScreen[i].iMaxFrame)
                m_pScreen[i].iCurrentFrameIndex = 0;
        }
        m_fFrameElapsedTime = 0.f;
    }
}

void CScottScreen::Late_Update(_float dt)
{
}

CScottScreen* CScottScreen::Create()
{
    CScottScreen* pInstance = new CScottScreen();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to create : CScottScreen");
    }

    return pInstance;
}

CGameObject* CScottScreen::Clone(INIT_DESC* pArg)
{
    CScottScreen* pInstance = new CScottScreen(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to clone : CScottScreen");
    }

    return pInstance;
}

void CScottScreen::Free()
{
    __super::Free();
}
