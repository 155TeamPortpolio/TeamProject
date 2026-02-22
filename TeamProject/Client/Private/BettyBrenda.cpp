#include "pch.h"
#include "BettyBrenda.h"

#include "GameInstance.h"
#include "FieldSystem.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"

CBettyBrenda::CBettyBrenda()
	:CServiceNpc()
{
}

CBettyBrenda::CBettyBrenda(const CBettyBrenda& rhs)
	:CServiceNpc(rhs)
{
}

HRESULT CBettyBrenda::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "NPC_Female_BettyBrenda_Model_WithRootKeeper(Clone)out.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "NPC_Female_BettyBrenda_Model_WithRootKeeper(Clone)out.mat");

	return S_OK;
}

HRESULT CBettyBrenda::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBettyBrenda::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "NPC_Female_BettyBrenda_Model_WithRootKeeper(Clone)out.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Female_BettyBrenda_Meta.json");

	m_strAnimName = "NPC_Female_Size02_BettyBrenda_Ani_";
	m_strName = L"º£Æ¼";

	pAnimator->Set_Animation(Get_AnimName() + "Maincity_Idle")
		.Loop(true)
		.Apply();

	__super::Awake();
}

void CBettyBrenda::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBettyBrenda::Update(_float dt)
{
	__super::Update(dt);
}

void CBettyBrenda::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

CBettyBrenda* CBettyBrenda::Create()
{
	CBettyBrenda* instance = new CBettyBrenda();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CBettyBrenda");
	}

	return instance;
}

CGameObject* CBettyBrenda::Clone(INIT_DESC* pArg)
{
	CBettyBrenda* instance = new CBettyBrenda(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CBettyBrenda");
	}
	return instance;
}

void CBettyBrenda::Free()
{
	__super::Free();
}
