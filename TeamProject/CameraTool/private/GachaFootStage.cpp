#include "pch.h"
#include "GachaFootStage.h"
// Engine
#include "StaticModel.h"
#include "Material.h"

HRESULT CGachaFootStage::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	RES->Add_ResourcePath("GachaStage_Prop_AvatarPlatform_02out.model", "../bin/Resources/Gacha/Avatar/FootStage/GachaStage_Prop_AvatarPlatform_02out.model");
	RES->Add_ResourcePath("GachaStage_Prop_AvatarPlatform_02out.mat",   "../bin/Resources/Gacha/Avatar/FootStage/GachaStage_Prop_AvatarPlatform_02out.mat");

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CGachaFootStage::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);

	return S_OK;
}

void CGachaFootStage::Awake()
{
	Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "GachaStage_Prop_AvatarPlatform_02out.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "GachaStage_Prop_AvatarPlatform_02out.mat");
	Get_Component<CStaticModel>()->SetDrawable(33, false);
}

CGachaFootStage* CGachaFootStage::Create()
{
	auto inst = new CGachaFootStage();
	if (FAILED(inst->Initialize_Prototype()))
	{
		Safe_Release(inst);
		return nullptr;
	}
	return inst;
}

CGameObject* CGachaFootStage::Clone(INIT_DESC* pArg)
{
	auto inst = new CGachaFootStage(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		Safe_Release(inst);
		return nullptr;
	}
	return inst;
}