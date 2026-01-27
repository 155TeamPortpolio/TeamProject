#include "pch.h"
#include "CDefiler.h"

#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Animator3D.h"
#include "Material.h"
#include "CharacterController.h"

#include "StateMachine.h"

CDefiler::CDefiler()
	:CEnemy()
{
}

CDefiler::CDefiler(const CDefiler& rhg)
	:CEnemy(rhg)
{
}

HRESULT CDefiler::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	Get_Component<CSkeletalModel>()->Link_Model(G_GlobalLevelKey, "Defiler_Isolde.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Defiler_Isolde.mat");

	return S_OK;
}

HRESULT CDefiler::Initialize(INIT_DESC* pArg)
{
	m_eEnemyClass = ENEMY_CLASS::BOSS;

	__super::Initialize(pArg);

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Defiler_Isolde.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Defiler_Isolde_Meta.json");
	pAnimator->Resize_Layer(3);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 2);

	auto pCCT = Get_Component<CCharacterController>();

	//if (FAILED(Initialize_StateMachine()))
	//	return E_FAIL;
	//
	//if (FAILED(Create_Colliders()))
	//	return E_FAIL;


	Create_UIEnemyStatus("Bip001_Spine2");
	Create_UIBossHUD();

	return S_OK;
}

void CDefiler::Awake()
{
}

void CDefiler::Priority_Update(_float dt)
{
}

void CDefiler::Update(_float dt)
{
	__super::Update(dt);

	//Update_States(dt);
	m_pStateMachine->Update(dt);

	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);
}

void CDefiler::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CDefiler::Render_GUI()
{
	__super::Render_GUI();

	Render_GUI_ForTargetInfo();
	m_pStateMachine->Render_GUI();
	ImGui::Text("Current State : %s", m_pStateMachine->Get_CurrentStateName().c_str());
}

CDefiler* CDefiler::Create()
{
	CDefiler* instance = new CDefiler();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefiler");
	}

	return instance;
}

CGameObject* CDefiler::Clone(INIT_DESC* pArg)
{
	CDefiler* instance = new CDefiler(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefiler");
	}

	return instance;
}

void CDefiler::Free()
{
	__super::Free();
	Safe_Release(m_pStateMachine);
}
