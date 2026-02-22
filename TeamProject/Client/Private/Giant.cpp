#include "pch.h"
#include "Giant.h"

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
#include "AudioSource.h"

/* States */
#include "StateMachine.h"
#include "Giant_Attack.h"
#include "Giant_Born.h"
#include "Giant_Death.h"
#include "Giant_Groggy.h"
#include "Giant_Hit.h"
#include "Giant_Idle.h"
#include "Giant_Move.h"
#include "Giant_Chase.h"
#include "Giant_Parried.h"

#include "EffectContainer.h"

CGiant::CGiant()
	: CEnemyNormal()
{
}

CGiant::CGiant(const CGiant& rhg)
	: CEnemyNormal(rhg)
{
}

HRESULT CGiant::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("Monster_Giant.mat", "../Bin/Resources/Zero/Enemy/Giant/Monster_Giant.mat");
	pResourceMgr->Add_ResourcePath("Monster_Giant.model", "../Bin/Resources/Zero/Enemy/Giant/Monster_Giant.model");
	//pResourceMgr->Add_ResourcePath("Monster_Claymore_Meta.json", "../../Resources/Data/Meta/Zero/Monster_Claymore_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Monster_Giant.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Monster_Giant.mat");

	return S_OK;
}

HRESULT CGiant::Initialize(INIT_DESC* pArg)
{
	m_eEnemyClass = ENEMY_CLASS::ELITE;

	__super::Initialize(pArg);

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Monster_Giant.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_Giant_Meta.json");
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	pAnimator->Resize_Layer(2);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);

	if (FAILED(Ready_Children(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	if (FAILED(Initialize_Effects()))
		return E_FAIL;

	Get_Component<CCharacterController>()->Set_StepOffset(0.01f);

	Get_Component<CAudioSource>()->SoundFolder(LevelManager()->Get_NowLevelKey(), "../Bin/Resources/Zero/Enemy/Giant/Sound");
	Get_Component<CAudioSource>()->Slot("giant_Spawn.wav").Attribute3D(true).Play();

	return S_OK;
}

void CGiant::Awake()
{
	__super::Awake();
}

void CGiant::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CGiant::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);

	__super::Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CGiant::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);

	__super::Late_Update(dt);
}

void CGiant::Render_GUI()
{
	ImGui::PushID(this);

	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);
	
	GUI_DebugButton();

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

		if (ImGui::TreeNode("AttackState##ThugAssaulterTestState_Attack"))
		{
			_bool	isClicked = false;
			_int	iClickedIndex = {};

			if (ImGui::Button("Attack1,"))
			{
				isClicked = true;
				iClickedIndex = 1;
			}
			if (ImGui::Button("Attack2,"))
			{
				isClicked = true;
				iClickedIndex = 2;
			}
			if (ImGui::Button("Attack2_1,"))
			{
				isClicked = true;
				iClickedIndex = 3;
			}
			if (ImGui::Button("Attack2_Explode,"))
			{
				isClicked = true;
				iClickedIndex = 4;
			}
			if (ImGui::Button("Attack3,"))
			{
				isClicked = true;
				iClickedIndex = 5;
			}
			if (ImGui::Button("Attack3_HitWall,"))
			{
				isClicked = true;
				iClickedIndex = 6;
			}
			if (ImGui::Button("Attack4,"))
			{
				isClicked = true;
				iClickedIndex = 7;
			}
			if (ImGui::Button("Attack5,"))
			{
				isClicked = true;
				iClickedIndex = 8;
			}
			//if (ImGui::Button("Attack6_AttackBack,"))
			//{
			//	isClicked = true;
			//	iClickedIndex = 9;
			//}
			//if (ImGui::Button("Attack7,"))
			//{
			//	isClicked = true;
			//	iClickedIndex = 10;
			//}
			//if (ImGui::Button("Attack7_Jump,"))
			//{
			//	isClicked = true;
			//	iClickedIndex = 11;
			//}
			//if (ImGui::Button("Attack7_Revenge"))
			//{
			//	isClicked = true;
			//	iClickedIndex = 1;
			//}

			if (isClicked)
			{
				m_pStateMachine->Set_Int("AttackPattern", iClickedIndex);
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}

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

void CGiant::Parried()
{
	if ("Attack" != m_pStateMachine->Get_CurrentStateName()/* || false == m_isParryEnable*/)
		return;

	__super::Parried();

	if (false == m_isParryDontStop)
		m_pStateMachine->Change_State("Parried");

	SetOnAttack(false, ATTACK_SIDE::NONE); 
	SetBattleColliderObject("Weapon_L", BATTLE_COLTYPE::ATTACK, false);
	SetBattleColliderObject("Weapon_R", BATTLE_COLTYPE::ATTACK, false);

	Get_Component<CAudioSource>()->Set_SlotStop("giant_Attack1_FULL.wav");
	//Get_Component<CAudioSource>()->Set_SlotStop("giant_Attack2_FULL.wav");
	//Get_Component<CAudioSource>()->Set_SlotStop("giant_Attack2_1_FULL.wav");
	//Get_Component<CAudioSource>()->Set_SlotStop("giant_Attack2_Explode_FULL.wav");
	Get_Component<CAudioSource>()->Set_SlotStop("giant_Attack3_FULL.wav");
	Get_Component<CAudioSource>()->Set_SlotStop("giant_Attack4_FULL.wav");
	//Get_Component<CAudioSource>()->Set_SlotStop("giant_Attack5_FULL.wav");

}

HRESULT CGiant::Ready_Children(INIT_DESC* pArg)
{
	{
		BATTLE_COLLIDER_DESC WeaponLDesc = {};

		WeaponLDesc.tagName = "Weapon_L";
		WeaponLDesc.isAttachBone = true;
		WeaponLDesc.tagBone = "Bip001_L_Hand";
		WeaponLDesc.pOwnerAnimator3D = Get_Component<CAnimator3D>();
		WeaponLDesc.vAttackSize = { 2.f, 0.3f, 0.3f };

		if (FAILED(AttachBattleColliderObject(&WeaponLDesc)))
			return E_FAIL;
	}
	{
		BATTLE_COLLIDER_DESC WeaponRDesc = {};

		WeaponRDesc.tagName = "Weapon_R";
		WeaponRDesc.isAttachBone = true;
		WeaponRDesc.tagBone = "Bip001_R_Hand";
		WeaponRDesc.pOwnerAnimator3D = Get_Component<CAnimator3D>();
		WeaponRDesc.vAttackSize = { 2.f, 0.3f, 0.3f };

		if (FAILED(AttachBattleColliderObject(&WeaponRDesc)))
			return E_FAIL;
	}

	Create_AttackSign("Bip001_Head");
	Create_UIEnemyStatus("Bip001_Spine2");
	Create_MeshPyramid();

	return S_OK;
}

CGiant* CGiant::Create()
{
	CGiant* instance = new CGiant();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CGiant");
	}

	return instance;
}

CGameObject* CGiant::Clone(INIT_DESC* pArg)
{
	CGiant* instance = new CGiant(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CGiant");
	}

	return instance;
}

void CGiant::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}


void CGiant::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	__super::TakeDamage(eDamageType, fDamage, charaName);

	if (0 >= m_tStatus.iNowHP)
		return;

	if ("Groggy" == m_pStateMachine->Get_CurrentStateName())
	{
		Get_Component<CAnimator3D>()->Set_Animation(1, "Giant_Ani_Hit_Stay")
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
		Get_Component<CAnimator3D>()->Set_Animation(1, "Giant_Ani_Hit_Stay")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::Linear)
			.Loop(false)
			.Apply();
	}
}

/* For.State Machine */
HRESULT CGiant::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CGiant>::Create();
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

	Get_Component<CAnimator3D>()->Set_Animation("Giant_Ani_Born")
		.Apply();

	return S_OK;
}

HRESULT CGiant::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CGiant_Born::Create());
	m_pStateMachine->Register_State("Idle", CGiant_Idle::Create());
	m_pStateMachine->Register_State("Attack", CGiant_Attack::Create());
	m_pStateMachine->Register_State("Move", CGiant_Move::Create());
	m_pStateMachine->Register_State("Chase", CGiant_Chase::Create());
	m_pStateMachine->Register_State("Death", CGiant_Death::Create());
	m_pStateMachine->Register_State("Groggy", CGiant_Groggy::Create());
	m_pStateMachine->Register_State("Hit", CGiant_Hit::Create());
	m_pStateMachine->Register_State("Parried", CGiant_Parried::Create());

	return S_OK;
}

HRESULT CGiant::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CGiant>::CONDITION_ANIMATION_END);
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CGiant>::CONDITION_TRIGGER, "Idle_To_Attack");
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CGiant>::CONDITION_TRIGGER, "Idle_To_Move");
	m_pStateMachine->Register_Transition("Idle", "Chase",
		CStateMachine<CGiant>::CONDITION_TRIGGER, "Idle_To_Chase");
	m_pStateMachine->Register_Transition("Idle", "Death",
		CStateMachine<CGiant>::CONDITION_TRIGGER, "Idle_To_Death");
	m_pStateMachine->Register_Transition("Idle", "Groggy",
		CStateMachine<CGiant>::CONDITION_TRIGGER, "Idle_To_Groggy");
	m_pStateMachine->Register_Transition("Idle", "Hit",
		CStateMachine<CGiant>::CONDITION_TRIGGER, "Idle_To_Hit");

	return S_OK;
}

HRESULT CGiant::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	for (_uint i = 0; i < 3; ++i)
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("giant_hit_ground0.json")
			.Build("Giant_HitGround0_" + to_string(i));
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}
	for (_uint i = 0; i < 3; ++i)
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("giant_hit_ground1.json")
			.Build("Giant_HitGround1_" + to_string(i));
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}
	for (_uint i = 0; i < 2; ++i)
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("giant_slash0.json")
			.Build("Giant_Slash0_" + to_string(i));
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("giant_dash_trail.json")
			.Build("Giant_Dash_Trail");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}


	return S_OK;
}

HRESULT CGiant::Ready_Rules()
{
	// x = Idle에서 다음 상태로 넘어가는 쿨타임, y = dt 더한 타이머용
	m_vIdleTime = { 0.2f, 0.f };

	// 생각보다 가까움. 다시짜자
	// 제일 가까이 붙으면 1.5f
	
	m_tHysteriesis.fEvadeEnter = 3.f;
	m_tHysteriesis.fComboEnter = 4.f;		// Attack1, Attack2
	m_tHysteriesis.fComboExit = 12.f;		// Attack2_1, Attack3
	m_tHysteriesis.fChaseEnter = 13.f;
	m_tHysteriesis.fChaseExit = 8.f; ;		//  Attack2_Explode, Attack3, Attack4, Attack5

	return S_OK;
}

void CGiant::Update_States(_float dt)
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
	PlaySoundFromMeta();

	//================================
	ControlState(dt);
	//================================
}

void CGiant::ControlState(const _float dt)
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
	{
		m_pStateMachine->Change_State("Groggy");
		DIR eDir = GetDIRToPlayer();
		m_pStateMachine->Set_Int("Dir", ENUM(eDir));
	}


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

void CGiant::CheckDistanceFromPlayer()
{
	if ("Chase" != m_pStateMachine->Get_CurrentStateName() &&
		m_tTargetingInfo.fDistance >= m_tHysteriesis.fChaseEnter)
		m_pStateMachine->Set_Bool("Chase", true);

	if (true == m_pStateMachine->Get_Bool("Chase") &&
		m_tTargetingInfo.fDistance <= m_tHysteriesis.fChaseExit)
		m_pStateMachine->Set_Bool("Chase", false);
}

void CGiant::ManageAttackHistory()
{
	_uint iSize = static_cast<_uint>(m_AttackHistory.size());
	if (5 <= iSize)
		m_AttackHistory.pop_back();
}