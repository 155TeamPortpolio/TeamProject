#include "pch.h"
#include "Avatar.h"

#include "SkeletalModel.h"
#include "Animator3D.h"

#include "Model.h"
#include "Material.h"

HRESULT CAvatar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	RES->Add_ResourcePath("Corin.model", "../bin/Resources/Corin/Corin.model");
	RES->Add_ResourcePath("Corin.mat", "../bin/Resources/Corin/Corin.mat");
	RES->Add_ResourcePath("Corin_Meta.json", "../bin/Resources/Corin/Corin_Meta.json");

	Add_Component<CSkeletalModel>()->Link_Model("First_Level","Corin.model");
	Add_Component<CMaterial>()->Link_Material("First_Level", "Corin.mat");
	Add_Component<CAnimator3D>();

	return S_OK;
}

HRESULT CAvatar::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CAvatar::Awake()
{
	__super::Awake();

	Get_Component<CAnimator3D>()->LinkAnimate_Model("Test_Level", "Corin.model");
	Get_Component<CAnimator3D>()->Link_MetaData("Test_Level", "Avatar_Female_Size01_Corin_Meta.json");
}

void CAvatar::Priority_Update(_float dt)
{
}

void CAvatar::Update(_float dt)
{
}

void CAvatar::Late_Update(_float dt)
{
}

void CAvatar::Render_GUI()
{
}
