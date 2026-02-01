#include "pch.h"
#include "GachaBack.h"
// Engine
#include "StaticModel.h"
#include "Material.h"
#include "Child.h"

HRESULT CGachaBack::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	RES->Add_ResourcePath("GachaStageout.model", "../bin/Resources/Gacha/Stage/GachaStageout.model");
	RES->Add_ResourcePath("GachaStageout.mat", "../bin/Resources/Gacha/Stage/GachaStageout.mat");

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CGachaBack::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);
	return S_OK;
}

void CGachaBack::Awake()
{
	Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "GachaStageout.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "GachaStageout.mat");
}

CGachaBack* CGachaBack::Create()
{
	auto inst = new CGachaBack();
	if (FAILED(inst->Initialize_Prototype()))
	{
		Safe_Release(inst);
		return nullptr;
	}
	return inst;
}

CGameObject* CGachaBack::Clone(INIT_DESC* pArg)
{
	auto inst = new CGachaBack(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		Safe_Release(inst);
		return nullptr;
	}
	return inst;
}