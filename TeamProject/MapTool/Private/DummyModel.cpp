#include "pch.h"
#include "DummyModel.h"
#include "StaticModel.h"
#include "Material.h"

#include "GameInstance.h"

CDummyModel::CDummyModel()
{
}

CDummyModel::CDummyModel(const CDummyModel& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDummyModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	//Add_Component<CStaticModel>();
	//Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CDummyModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CDummyModel::Awake()
{
	/*CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("", "");
	Get_Component<CModel>()->Link_Model("Demo_Level", "");*/
}

void CDummyModel::Priority_Update(_float dt)
{
}

void CDummyModel::Update(_float dt)
{
}

void CDummyModel::Late_Update(_float dt)
{
}

void CDummyModel::Render_GUI()
{
	__super::Render_GUI();
}

CDummyModel* CDummyModel::Create()
{
	CDummyModel* instance = new CDummyModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDummyModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDummyModel::Clone(INIT_DESC* pArg)
{
	CDummyModel* instance = new CDummyModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDummyModel");
		Safe_Release(instance);
	}

	return instance;
}

void CDummyModel::Free()
{
	__super::Free();
}