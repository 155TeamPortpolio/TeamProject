#include "pch.h"
#include "DemoModel.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"

HRESULT CDemoModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
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
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	/*파일명과 키값은 일치*/
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).model",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).model");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).mat",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).mat");

	Get_Component<CModel>()->Link_Model("First_Level", "Bangboo_Sharkboo_NPC (merge).model");
	Get_Component<CMaterial>()->Link_Material("First_Level", "Bangboo_Sharkboo_NPC (merge).mat");
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