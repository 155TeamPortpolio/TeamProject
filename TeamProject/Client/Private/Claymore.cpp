#include "pch.h"
#include "Claymore.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Child */
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
#include "Claymore_Attack.h"
#include "Claymore_Born.h"
#include "Claymore_Death.h"
#include "Claymore_Groggy.h"
#include "Claymore_Hit.h"
#include "Claymore_Idle.h"
#include "Claymore_Move.h"
#include "Claymore_Chase.h"
#include "Claymore_Parried.h"

CClaymore::CClaymore()
	: CEnemyNormal()
{
}

CClaymore::CClaymore(const CClaymore& rhg)
	: CEnemyNormal(rhg)
{
}

HRESULT CClaymore::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("Claymore.mat", "../Bin/Resources/Zero/Enemy/Claymore/Claymore.mat");
	pResourceMgr->Add_ResourcePath("Claymore.model", "../Bin/Resources/Zero/Enemy/Claymore/Claymore.model");
	//pResourceMgr->Add_ResourcePath("Monster_Claymore_Meta.json", "../../Resources/Data/Meta/Zero/Monster_Claymore_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Claymore.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Claymore.mat");

	return S_OK;
}

HRESULT CClaymore::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Claymore.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_Claymore_Meta.json");
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	pAnimator->Resize_Layer(2);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);

	if (FAILED(Ready_Children(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CClaymore::Awake()
{
	__super::Awake();
}

void CClaymore::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CClaymore::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);

	__super::Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CClaymore::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);

	__super::Late_Update(dt);
}

void CClaymore::Render_GUI()
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


#pragma region Status
	ImGui::SeparatorText("Status");
	auto pCharacter = GetCharacterOnField();
	if (nullptr != pCharacter) {
		ImGui::BeginChild("TracePlayer##ThugAssaulterStatus", ImVec2{ 0, childHeight + textLineHeight * 4.f }, true);

		ImGui::Text("AnimName : %s", Get_Component<CAnimator3D>()->Get_CurAnimName().c_str());
		ImGui::Text("SelfDir: %.2f, %.2f, %.2f", m_tTargetingInfo.vDirSelfLook.x, m_tTargetingInfo.vDirSelfLook.y, m_tTargetingInfo.vDirSelfLook.z);
		ImGui::Text("CaptureDir: %.2f, %.2f, %.2f", m_tRotDir.vDirToLookCapture.x, m_tRotDir.vDirToLookCapture.y, m_tRotDir.vDirToLookCapture.z);
		ImGui::Text("HP : %d", (_int)m_tStatus.iNowHP);
		ImGui::Text("Groggy Value : %d", m_tStatus.iGroggyValue);
		ImGui::Text("Groggy StayTime : %d", m_tGroggyManage.fGroggyStayTime);

		ImGui::BeginDisabled(true);
		//ImGui::Checkbox(u8"isLookPlayer", &m_isLookPlayer);
		ImGui::Checkbox("IsGroggy", &m_tStatus.isGroggy);
		ImGui::Checkbox("ForUI.IsGroggyStay", &m_tStatus.isGroggyStay);
		ImGui::Checkbox("IsOnAttack", &m_isOnAttack);
		ImGui::EndDisabled();

		ImGui::EndChild();
	}
#pragma endregion

#pragma region TargetInfo
	Render_GUI_ForTargetInfo();
#pragma endregion

#pragma region CheckState
	if (ImGui::TreeNode("Test State##ThugAssaulterCheckState")) {
		//ImGui::BeginChild("State##ThugBulkyEnforcerStatus", ImVec2{ 0, childHeight }, true);

		if (ImGui::TreeNode("AttackState##ThugAssaulterTestState_Attack"))
		{
			if (ImGui::Button(u8"1. Attack01"))
			{
				m_pStateMachine->Set_Int("AttackPattern", 1);
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}
			if (ImGui::Button(u8"2. Attack02"))
			{
				m_pStateMachine->Set_Int("AttackPattern", 2);
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}
			if (ImGui::Button(u8"3. Attack03"))
			{
				m_pStateMachine->Set_Int("AttackPattern", 3);
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Death##ThugAssaulterTestDeath"))
		{
			if (ImGui::Button("Death Front"))
				m_pStateMachine->Change_State("Death");

			if (ImGui::Button("Death Back"))
			{
				m_pStateMachine->Set_Bool("DeathBack", true);
				m_pStateMachine->Change_State("Death");
			}

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Groggy&Hit##ThugAssaulterTestGroggy&Hit"))
		{
			if (ImGui::Button("Increase Groggy value 30"))
				m_tStatus.iGroggyValue += 30;

			if (ImGui::Button("Hit"))
				TakeDamage(DAMAGE_TYPE::NORMAL, 20.f);

			if (ImGui::Button("Parried"))
				Parried();

			if (ImGui::Button("Execute"))
				m_tStatus.iNowHP -= m_tStatus.iMaxHP;



			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
#pragma endregion

#pragma region AutoPattern
	ImGui::Checkbox("Auto Pattern", &m_isAutoPatternPlay);
#pragma endregion



	ImGui::PopID();
}

void CClaymore::OnPooledAcquire(INIT_DESC* pArg)
{
	Initialize(pArg);
}

void CClaymore::OnPooledRelease()
{
}

void CClaymore::Parried()
{
	if ("Attack" != m_pStateMachine->Get_CurrentStateName() || false == m_isParryEnable)
		return;

	__super::Parried();

	m_pStateMachine->Change_State("Parried");
	SetOnAttack(false, ATTACK_SIDE::NONE); 
}

HRESULT CClaymore::Ready_Children(INIT_DESC* pArg)
{
	BATTLE_COLLIDER_DESC WeaponDesc = {};

	WeaponDesc.tagName = "Weapon";
	WeaponDesc.isAttachBone = true;
	WeaponDesc.tagBone = "Bip001_L_Forearm";
	WeaponDesc.pOwnerAnimator3D = Get_Component<CAnimator3D>();
	WeaponDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
	WeaponDesc.vCenter = { 0.8f, 0.f, 0.f };
	WeaponDesc.vAttackSize = { 1.8f, 0.3f, 0.3f };

	if (FAILED(AttachBattleColliderObject(&WeaponDesc)))
		return E_FAIL;

	Create_AttackSign("Bip001_Head");
	Create_UIEnemyStatus("Bip001_Spine2");
	Create_MeshPyramid();

	return S_OK;
}

CClaymore* CClaymore::Create()
{
	CClaymore* instance = new CClaymore();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CClaymore");
	}

	return instance;
}

CGameObject* CClaymore::Clone(INIT_DESC* pArg)
{
	CClaymore* instance = new CClaymore(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CClaymore");
	}

	return instance;
}

void CClaymore::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}


void CClaymore::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	__super::TakeDamage(eDamageType, fDamage, charaName);

	if (0 >= m_tStatus.iNowHP)
		return;

	if ("Groggy" == m_pStateMachine->Get_CurrentStateName())
	{
		Get_Component<CAnimator3D>()->Set_Animation(1, "Claymore_Ani_Hit_Stay")
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
		Get_Component<CAnimator3D>()->Set_Animation(1, "Claymore_Ani_Hit_Stay")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::Linear)
			.Loop(false)
			.Apply();
	}
}

/* For.State Machine */
HRESULT CClaymore::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CClaymore>::Create();
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

	Get_Component<CAnimator3D>()->Set_Animation("Monster_Claymore_Ani_Born")
		.Apply();

	return S_OK;
}

HRESULT CClaymore::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CClaymore_Born::Create());
	m_pStateMachine->Register_State("Idle", CClaymore_Idle::Create());
	m_pStateMachine->Register_State("Attack", CClaymore_Attack::Create());
	m_pStateMachine->Register_State("Move", CClaymore_Move::Create());
	m_pStateMachine->Register_State("Chase", CClaymore_Chase::Create());
	m_pStateMachine->Register_State("Death", CClaymore_Death::Create());
	m_pStateMachine->Register_State("Groggy", CClaymore_Groggy::Create());
	m_pStateMachine->Register_State("Hit", CClaymore_Hit::Create());
	m_pStateMachine->Register_State("Parried", CClaymore_Parried::Create());

	return S_OK;
}

HRESULT CClaymore::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CClaymore>::CONDITION_ANIMATION_END);
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Attack");
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Move");
	m_pStateMachine->Register_Transition("Idle", "Chase",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Chase");
	m_pStateMachine->Register_Transition("Idle", "Death",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Death");
	m_pStateMachine->Register_Transition("Idle", "Groggy",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Groggy");
	m_pStateMachine->Register_Transition("Idle", "Hit",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Hit");

	return S_OK;
}

HRESULT CClaymore::Ready_Rules()
{
	// x = Idle에서 다음 상태로 넘어가는 쿨타임, y = dt 더한 타이머용
	m_vIdleTime = { 1.f, 0.f };

	m_tHysteriesis.fEvadeEnter = 2.f;
	m_tHysteriesis.fComboEnter = 3.f;
	m_tHysteriesis.fComboExit = 4.5f;
	m_tHysteriesis.fChaseEnter = 7.f;
	m_tHysteriesis.fChaseExit = 5.f;

	return S_OK;
}

void CClaymore::Update_States(_float dt)
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

void CClaymore::ControlState(const _float dt)
{
	if ("Death" != m_pStateMachine->Get_CurrentStateName() &&
		0 >= m_tStatus.iNowHP)
	{
		RequestRemoveOnDeathToBattleSystem();
		m_pStateMachine->Change_State("Death");
	}

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

void CClaymore::CheckDistanceFromPlayer()
{
	if ("Chase" != m_pStateMachine->Get_CurrentStateName() &&
		m_tTargetingInfo.fDistance >= m_tHysteriesis.fChaseEnter)
		m_pStateMachine->Set_Bool("Chase", true);

	if (true == m_pStateMachine->Get_Bool("Chase") &&
		m_tTargetingInfo.fDistance <= m_tHysteriesis.fChaseExit)
		m_pStateMachine->Set_Bool("Chase", false);
}