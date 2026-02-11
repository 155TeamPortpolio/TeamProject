#include "pch.h"
#include "Water.h"

#include "StaticModel.h"
#include "Material.h"

CWater::CWater()
    :CGameObject()
{
}

CWater::CWater(const CWater& rhs)
    :CGameObject(rhs)
{
}

HRESULT CWater::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CStaticModel>();
    Add_Component<CMaterial>();
    return S_OK;
}

HRESULT CWater::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto pModel = Get_Component<CStaticModel>();
    auto pMaterial = Get_Component<CMaterial>();

    pModel->Link_Model("Zero_Level", "ZeroWater.model");
    pMaterial->Link_Material("Zero_Level", "ZeroWater.mat");

    pModel->ShadowCast(false);
    return S_OK;
}

void CWater::Awake()
{
    auto pMaterial = Get_Component<CMaterial>();
    auto MaterialInstances = pMaterial->Get_MaterialInstances();
    for (auto& Instance : MaterialInstances)
    {
        Instance->Set_Blended(true);
        Instance->Set_Param("g_Time", { &m_fAccTime, "float", sizeof(_float) });
    }
}

void CWater::Priority_Update(_float dt)
{
}

void CWater::Update(_float dt)
{
	m_fAccTime += dt;
}

void CWater::Late_Update(_float dt)
{
}

CWater* CWater::Create()
{
	CWater* Instance = new CWater();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CWater::Clone(INIT_DESC* pArg)
{
	CWater* Instance = new CWater(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CWater::Free()
{
	__super::Free();
}
