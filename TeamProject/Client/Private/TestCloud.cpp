#include "pch.h"
#include "TestCloud.h"

#include "StaticModel.h"
#include "Material.h"

#include "GameInstance.h"

CTestCloud::CTestCloud()
{
}

CTestCloud::CTestCloud(const CTestCloud& rhs)
	:CGameObject(rhs)
{
}

HRESULT CTestCloud::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CMaterial>();
	Add_Component<CStaticModel>();

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("CloudTest.model",
		"../Bin/Resources/Cloud/CloudTest.model");
	pRcsMgr->Add_ResourcePath("CloudTest.mat",
		"../Bin/Resources/Cloud/CloudTest.mat");
	Get_Component<CStaticModel>()->Link_Model("Test_Level", "CloudTest.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "CloudTest.mat");

	return S_OK;
}

HRESULT CTestCloud::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CTestCloud::Awake()
{
}

void CTestCloud::Priority_Update(_float dt)
{
}

void CTestCloud::Update(_float dt)
{
}

void CTestCloud::Late_Update(_float dt)
{
}

CTestCloud* CTestCloud::Create()
{
	CTestCloud* Instance = new CTestCloud();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CTestCloud");
		Safe_Release(Instance);
	}

	return Instance;
}

CGameObject* CTestCloud::Clone(INIT_DESC* pArg)
{
	CTestCloud* Instance = new CTestCloud(*this);

	if (FAILED(Instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CTestCloud");
		Safe_Release(Instance);
	}

	return Instance;
}

void CTestCloud::Free()
{
	__super::Free();
}
