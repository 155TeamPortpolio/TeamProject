#include "pch.h"
#include "GachaTV.h"
// Engine
#include "StaticModel.h"
#include "Material.h"

HRESULT CGachaTV::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto hr1 = RES->Add_ResourcePath("GachaStage_Televisonout.model", "../bin/Resources/Gacha/Television/GachaStage_Televisonout.model");
	auto hr2 = RES->Add_ResourcePath("GachaStage_Televisonout.mat",   "../bin/Resources/Gacha/Television/GachaStage_Televisonout.mat");

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CGachaTV::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);

	return S_OK;
}

void CGachaTV::Awake()
{
 	Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "GachaStage_Televisonout.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "GachaStage_Televisonout.mat");
	Get_Component<CStaticModel>()->SetDrawable(33, false);
}

CGachaTV* CGachaTV::Create()
{
	auto inst = new CGachaTV();
	if (FAILED(inst->Initialize_Prototype()))
	{
		Safe_Release(inst);
		return nullptr;
	}
	return inst;
}

CGameObject* CGachaTV::Clone(INIT_DESC* pArg)
{
	auto inst = new CGachaTV(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		Safe_Release(inst);
		return nullptr;
	}
	return inst;
}