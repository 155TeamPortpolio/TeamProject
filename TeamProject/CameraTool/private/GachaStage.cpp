#include "pch.h"
#include "GachaStage.h"
// Engine
#include "StaticModel.h"
#include "Material.h"

HRESULT CGachaStage::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	RES->Add_ResourcePath("GachaStage_Bangbooout.model", "../bin/Resources/Gacha/BangBoo/GachaStage_Bangbooout.model");
	RES->Add_ResourcePath("GachaStage_Bangbooout.mat",   "../bin/Resources/Gacha/BangBoo/GachaStage_Bangbooout.mat");
	RES->Add_ResourcePath("GachaStage_Avatarout.model",  "../bin/Resources/Gacha/Avatar/GachaStage_Avatarout.model");
	RES->Add_ResourcePath("GachaStage_Avatarout.mat",    "../bin/Resources/Gacha/Avatar/GachaStage_Avatarout.mat");

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CGachaStage::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);
	return S_OK;
}

void CGachaStage::Awake()
{
	Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "GachaStage_Avatarout.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "GachaStage_Avatarout.mat");

	Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "GachaStage_Bangbooout.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "GachaStage_Bangbooout.mat");
}

CGachaStage* CGachaStage::Create()
{
	auto inst = new CGachaStage();
	if (FAILED(inst->Initialize_Prototype()))
	{
		Safe_Release(inst);
		return nullptr;
	}
	return inst;
}

CGameObject* CGachaStage::Clone(INIT_DESC* arg)
{
	auto inst = new CGachaStage(*this);
	if (FAILED(inst->Initialize(arg)))
	{
		Safe_Release(inst);
		return nullptr;
	}
	return inst;
}