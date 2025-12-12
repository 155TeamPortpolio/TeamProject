#include "pch.h"
#include "DemoModel.h"
#include "StaticModel.h"
#include "Material.h"

#include "GameInstance.h"

CDemoModel::CDemoModel()
{
}

CDemoModel::CDemoModel(const CDemoModel& rhs)
	:CGameObject(rhs) 
{
}

HRESULT CDemoModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CDemoModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CDemoModel::Awake()
{
	CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("", "");
	Get_Component<CModel>()->Link_Model("Demo_Level", "");
}

void CDemoModel::Priority_Update(_float dt)
{
}

void CDemoModel::Update(_float dt)
{
}

void CDemoModel::Late_Update(_float dt)
{
}

void CDemoModel::Render_GUI()
{
	__super::Render_GUI();
}

CDemoModel* CDemoModel::Create()
{
	CDemoModel* instance = new CDemoModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoModel::Clone(INIT_DESC* pArg)
{
	CDemoModel* instance = new CDemoModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoModel");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoModel::Free()
{
	__super::Free();
}