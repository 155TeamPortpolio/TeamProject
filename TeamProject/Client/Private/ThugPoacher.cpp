#include "pch.h"
#include "ThugPoacher.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "AttackSign.h"

/* Component */
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ObjectContainer.h"
#include "CharacterController.h"
#include "BoneFollower.h"

/* States */
#include "StateMachine.h"
#include "ThugPoacher_Attack.h"
#include "ThugPoacher_Born.h"
#include "ThugPoacher_Death.h"
#include "ThugPoacher_Groggy.h"
#include "ThugPoacher_Hit.h"
#include "ThugPoacher_Idle.h"
#include "ThugPoacher_Move.h"
#include "ThugPoacher_Chase.h"

CThugPoacher::CThugPoacher()
	: CEnemyNormal()
{
}

CThugPoacher::CThugPoacher(const CThugPoacher& rhg)
	: CEnemyNormal(rhg)
{
}

HRESULT CThugPoacher::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("ThugPoacher.mat", "../Bin/Resources/Model/skeletal/Enemy/ThugPoacher/ThugPoacher.mat");
	pResourceMgr->Add_ResourcePath("ThugPoacher.model", "../Bin/Resources/Model/skeletal/Enemy/ThugPoacher/ThugPoacher.model");
	pResourceMgr->Add_ResourcePath("ThugPoacher_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/ThugPoacher/ThugPoacher_Meta.json");

	return S_OK;
}

HRESULT CThugPoacher::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	//m_pTransform->Scale({ 0.01f,0.01f,0.01f });

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "ThugPoacher.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "ThugPoacher.mat");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "ThugPoacher.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "ThugPoacher_Meta.json");
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	pAnimator->Resize_Layer(2);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);

	if (FAILED(Ready_Children(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	// 임시 확인용
#ifdef _USING_GUI
	CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;
#endif // _USING_GUI

	return S_OK;
}

void CThugPoacher::Awake()
{
	__super::Awake();
}

void CThugPoacher::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CThugPoacher::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);

	__super::Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CThugPoacher::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);

	__super::Late_Update(dt);
}

void CThugPoacher::Render_GUI()
{
	ImGui::PushID(this);

	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

#pragma region Component Inspector
	if (ImGui::TreeNode("Inspector##ThugBulkyInspector")) {
		__super::Render_GUI();
		ImGui::TreePop();
	}
#pragma endregion

	ImGui::PopID();
}

void CThugPoacher::OnPooledAcquire(INIT_DESC* pArg)
{
	Initialize(pArg);
}

void CThugPoacher::OnPooledRelease()
{
}

void CThugPoacher::Parried()
{
	/*if ("Attack" != m_pStateMachine->Get_CurrentStateName())
		return;

	__super::Parried();

	m_pStateMachine->Change_State("Parried");
	SetOnAttack(false, ATTACK_SIDE::NONE); */
}

HRESULT CThugPoacher::Ready_Children(INIT_DESC* pArg)
{
	BATTLE_COLLIDER_DESC WeaponDesc = {};

	WeaponDesc.tagName = "Weapon";
	WeaponDesc.isAttachBone = true;
	WeaponDesc.tagBone = "Bip_ThugAssaulter_Weapon";
	WeaponDesc.pOwnerAnimator3D = Get_Component<CAnimator3D>();
	WeaponDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
	WeaponDesc.vCenter = { 0.f, 0.f,-0.7f };
	WeaponDesc.vAttackSize = { 0.2f, 0.2f, 1.1f };
	WeaponDesc.vTriggerSize = { 4.f,0.f,0.f };

	if (FAILED(AttachBattleColliderObject(&WeaponDesc)))
		return E_FAIL;

	Create_AttackSign("Bip001_Head");
	Create_UIEnemyStatus("Bip001_Spine2");
	Create_MeshPyramid();

	return S_OK;
}

CThugPoacher* CThugPoacher::Create()
{
	CThugPoacher* instance = new CThugPoacher();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CThugPoacher");
	}

	return instance;
}

CGameObject* CThugPoacher::Clone(INIT_DESC* pArg)
{
	CThugPoacher* instance = new CThugPoacher(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CThugPoacher");
	}

	return instance;
}

void CThugPoacher::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}


void CThugPoacher::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage)
{
	__super::TakeDamage(eDamageType, fDamage);

	if (0 >= m_tStatus.iNowHP)
		return;

	if ("Groggy" == m_pStateMachine->Get_CurrentStateName())
	{
		Get_Component<CAnimator3D>()->Set_Animation(1, "ThugPoacher_Ani_Hit_Knock")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::Linear)
			.Loop(false)
			.Apply();
	}
	else if ("Idle" == m_pStateMachine->Get_CurrentStateName())
	{
		m_pStateMachine->Set_Trigger("Idle_To_Hit");
		DIR eDir = GetDIRToPlayer();
		m_pStateMachine->Set_Int("Dir", ENUM(eDir));
	}
	else
	{
		Get_Component<CAnimator3D>()->Set_Animation(1, "ThugPoacher_Ani_Hit_Stay")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::Linear)
			.Loop(false)
			.Apply();
	}
}

/* For.State Machine */
HRESULT CThugPoacher::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CThugPoacher>::Create();
	if (nullptr == m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	if (FAILED(Ready_Rules()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Born");
	m_pStateMachine->Initialize(this);

	Get_Component<CAnimator3D>()->Set_Animation("ThugAssaulter_Ani_Born")
		.Apply();

	return S_OK;
}

HRESULT CThugPoacher::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CThugPoacher_Born::Create());
	m_pStateMachine->Register_State("Idle", CThugPoacher_Idle::Create());
	m_pStateMachine->Register_State("Attack", CThugPoacher_Attack::Create());
	m_pStateMachine->Register_State("Move", CThugPoacher_Move::Create());
	m_pStateMachine->Register_State("Chase", CThugPoacher_Chase::Create());
	m_pStateMachine->Register_State("Death", CThugPoacher_Death::Create());
	m_pStateMachine->Register_State("Groggy", CThugPoacher_Groggy::Create());
	m_pStateMachine->Register_State("Hit", CThugPoacher_Hit::Create());

	return S_OK;
}

HRESULT CThugPoacher::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CThugPoacher>::CONDITION_ANIMATION_END);
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CThugPoacher>::CONDITION_TRIGGER, "Idle_To_Attack");
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CThugPoacher>::CONDITION_TRIGGER, "Idle_To_Move");
	m_pStateMachine->Register_Transition("Idle", "Chase",
		CStateMachine<CThugPoacher>::CONDITION_TRIGGER, "Idle_To_Chase");
	m_pStateMachine->Register_Transition("Idle", "Death",
		CStateMachine<CThugPoacher>::CONDITION_TRIGGER, "Idle_To_Death");
	m_pStateMachine->Register_Transition("Idle", "Groggy",
		CStateMachine<CThugPoacher>::CONDITION_TRIGGER, "Idle_To_Groggy");
	m_pStateMachine->Register_Transition("Idle", "Hit",
		CStateMachine<CThugPoacher>::CONDITION_TRIGGER, "Idle_To_Hit");

	return S_OK;
}

HRESULT CThugPoacher::Ready_Rules()
{
	// x = Idle에서 다음 상태로 넘어가는 쿨타임, y = dt 더한 타이머용
	m_vIdleTime = { 1.f, 0.f };

	//m_tHysteriesis.fEvadeEnter = 2.f;
	//m_tHysteriesis.fComboEnter = 3.f;
	//m_tHysteriesis.fComboExit = 4.f;
	//m_tHysteriesis.fChaseEnter = 7.f;
	//m_tHysteriesis.fChaseExit = 5.f;

	return S_OK;
}

void CThugPoacher::Update_States(_float dt)
{
	if (true == m_isIdle) {
		m_pStateMachine->Change_State("Idle");
		m_pStateMachine->Reset_Trigger("Idle_To_Attack");
		m_pStateMachine->Reset_Trigger("Idle_To_Move");
		m_pStateMachine->Reset_Trigger("Idle_To_Chase");
		m_pStateMachine->Reset_Trigger("Idle_To_Death");
		m_pStateMachine->Reset_Trigger("Idle_To_Hit");

		m_isIdle = false;
	}

	CheckDistanceFromPlayer();

	//================================
	ControlState(dt);
	//================================
}

void CThugPoacher::ControlState(const _float dt)
{
	if ("Death" != m_pStateMachine->Get_CurrentStateName() &&
		0 >= m_tStatus.iNowHP)
		m_pStateMachine->Change_State("Death");

	if ("Death" != m_pStateMachine->Get_CurrentStateName() &&
		"Groggy" != m_pStateMachine->Get_CurrentStateName() &&
		true == m_tStatus.isGroggy)
		m_pStateMachine->Change_State("Groggy");


	if (true == m_isAutoPatternPlay &&
		"Idle" == m_pStateMachine->Get_CurrentStateName())
	{

		m_vIdleTime.y += dt;

		if (m_vIdleTime.x <= m_vIdleTime.y)
		{
			if (true == m_pStateMachine->Get_Bool("Chase"))
				m_pStateMachine->Set_Trigger("Idle_To_Chase");
			else if (true == m_pStateMachine->Get_Bool("Death"))
				m_pStateMachine->Set_Trigger("Idle_To_Death");
			else if ("Attack" != m_pStateMachine->Get_PrevStateName())
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			else
				m_pStateMachine->Set_Trigger("Idle_To_Move");


			m_vIdleTime.y = 0.f;
		}
	}
}

void CThugPoacher::CheckDistanceFromPlayer()
{
	//if ("Chase" != m_pStateMachine->Get_CurrentStateName() &&
	//	m_tTargetingInfo.fDistance >= m_tHysteriesis.fChaseEnter)
	//	m_pStateMachine->Set_Bool("Chase", true);
	
	//if (true == m_pStateMachine->Get_Bool("Chase") &&
	//	m_tTargetingInfo.fDistance <= m_tHysteriesis.fChaseExit)
	//	m_pStateMachine->Set_Bool("Chase", false);
}