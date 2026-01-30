#include "pch.h"
#include "ThugAssaulter.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Component */
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ObjectContainer.h"
#include "CharacterController.h"
#include "BoneFollower.h"

/* States */
#include "StateMachine.h"
#include "ThugAssaulter_Born.h"
#include "ThugAssaulter_Idle.h"
#include "ThugAssaulter_Attack.h"
#include "ThugAssaulter_Chase.h"
#include "ThugAssaulter_Death.h"
#include "ThugAssaulter_Move.h"
#include "ThugAssaulter_Groggy.h"
#include "ThugAssaulter_Hit.h"
#include "ThugAssaulter_Parried.h"

#include "AttackSign.h"

CThugAssaulter::CThugAssaulter()
	: CEnemyNormal()
{
}

CThugAssaulter::CThugAssaulter(const CThugAssaulter& rhg)
	: CEnemyNormal(rhg)
{
}

HRESULT CThugAssaulter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("ThugAssaulter.mat", "../Bin/Resources/Model/skeletal/Enemy/ThugAssaulter/ThugAssaulter.mat");
	pResourceMgr->Add_ResourcePath("ThugAssaulter.model", "../Bin/Resources/Model/skeletal/Enemy/ThugAssaulter/ThugAssaulter.model");
	pResourceMgr->Add_ResourcePath("Monster_ThugAssaulter_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/ThugAssaulter/Monster_ThugAssaulter_Meta.json");

	return S_OK;
}

HRESULT CThugAssaulter::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	//m_pTransform->Scale({ 0.01f,0.01f,0.01f });

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "ThugAssaulter.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "ThugAssaulter.mat");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "ThugAssaulter.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_ThugAssaulter_Meta.json");
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	pAnimator->Resize_Layer(2);
	//for (size_t i = 1; i < 2; i++)
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);

	if (FAILED(Ready_Children(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CThugAssaulter::Awake()
{
	__super::Awake();
}

void CThugAssaulter::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CThugAssaulter::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);

	__super::Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);

}

void CThugAssaulter::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);

	__super::Late_Update(dt);
}

void CThugAssaulter::Render_GUI()
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

#pragma region State
	ImGui::SeparatorText("State & BlackBoard");
	ImGui::BeginChild("##ThugAssaulterStatus", ImVec2{ 0, childHeight }, true);
	ImGui::Text("Current State : %s", m_pStateMachine->Get_CurrentStateName().c_str());

	// bool 변수 확인용(수정 불가)
	ImGui::BeginDisabled(true);
	ImGui::EndDisabled();

	ImGui::EndChild();
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
		ImGui::Checkbox("IsParryEnable", &m_isParryEnable);
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
			if (ImGui::Button(u8"4. Attack04"))
			{
				m_pStateMachine->Set_Int("AttackPattern", 4);
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

#pragma region StateMachine
	if (ImGui::Button("Open StateMachine"))
		m_pStateMachine->Set_ShowWindow(true);
	m_pStateMachine->Render_GUI();
#pragma endregion

	if (ImGui::TreeNode("BattleSystem Evade VFX##EvadeVFX"))
	{
		if (ImGui::Button(u8"회피 효과##BattleSystemVFXEvade"))
			BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::EVADE);


		ImGui::TreePop();
	}
	//// BattleSystem 시간 확인용
	//if (ImGui::TreeNode("BattleSystem TimeScale Check##TimeScaleCheck"))
	//{
	//	ImGui::BeginChild("##BattleSystemTimeScaleCheck", ImVec2{ 0, childHeight + textLineHeight * 15.f }, true);
	//
	//	auto pTimeScales = BattleSystem()->GetTimeScales();
	//
	//	for (_uint i = 0; i < ENUM(CBattleSystem::BATTLE_OBJ_TYPE::ENVOBJECT); i++)
	//	{
	//		auto tTimeScale = (*pTimeScales)[i];
	//
	//		if (i == 0)
	//			ImGui::Text("TagLayer : PLAYER");
	//		else {
	//			ImGui::Separator();
	//			ImGui::Text("TagLayer : Monster");
	//		}
	//
	//		ImGui::Text("Scale Value : %.2f", tTimeScale.fScaleValue);
	//		ImGui::Text("Duration : %.2f", tTimeScale.fDuration);
	//		ImGui::Text("Current Pos : %.2f", tTimeScale.fCurPos);
	//
	//		ImGui::BeginDisabled(true);
	//		string tagCheckBox = "isRunning##" + to_string(i) + "Checkbox";
	//		ImGui::Checkbox(tagCheckBox.c_str(), &tTimeScale.isRunning);
	//		string tagSlide = to_string(i) + "##Playback";
	//		ImGui::SliderFloat(tagSlide.c_str(), &tTimeScale.fCurPos, 0.f, tTimeScale.fDuration, "");
	//		ImGui::EndDisabled();
	//
	//	}
	//	ImGui::DragFloat("Scale Value##scalevalue", &m_fTestScaleValue, 0.1f);
	//	ImGui::DragFloat("Scale Duration##scaleDuration", &m_fTestScaleDuration, 0.1f);
	//
	//	if (ImGui::Button("Player TimeScale"))
	//		BattleSystem()->StartTimeScale(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER, m_fTestScaleDuration, m_fTestScaleValue, 0.f, 0.f);
	//
	//	ImGui::SameLine(0.f, 10.f);
	//	if (ImGui::Button("Monster TimeScale"))
	//		BattleSystem()->StartTimeScale(CBattleSystem::BATTLE_OBJ_TYPE::MONSTER, m_fTestScaleDuration, m_fTestScaleValue, 0.f, 0.f);
	//
	//	ImGui::EndChild();
	//	ImGui::TreePop();
	//}

	ImGui::PopID();
}

void CThugAssaulter::OnPooledAcquire(INIT_DESC* pArg)
{
	Initialize(pArg);
}

void CThugAssaulter::OnPooledRelease()
{
}

void CThugAssaulter::Parried()
{
	if ("Attack" != m_pStateMachine->Get_CurrentStateName())
		return;

	__super::Parried();

	m_pStateMachine->Change_State("Parried");
	SetOnAttack(false, ATTACK_SIDE::NONE);
}

HRESULT CThugAssaulter::Ready_Children(INIT_DESC* pArg)
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

CThugAssaulter* CThugAssaulter::Create()
{
	CThugAssaulter* instance = new CThugAssaulter();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CThugAssaulter");
	}

	return instance;
}

CGameObject* CThugAssaulter::Clone(INIT_DESC* pArg)
{
	CThugAssaulter* instance = new CThugAssaulter(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CThugAssaulter");
	}

	return instance;
}

void CThugAssaulter::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

void CThugAssaulter::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	__super::TakeDamage(eDamageType, fDamage, charaName);
	if (0 >= m_tStatus.iNowHP)
		return;


	switch (eDamageType)
	{
	case Client::DAMAGE_TYPE::NORMAL:
	{

		break;
	}
	case Client::DAMAGE_TYPE::HARD:
	{

		break;
	}
	case Client::DAMAGE_TYPE::AIRBORNE:
	{

		break;
	}
	}

	if ("Groggy" == m_pStateMachine->Get_CurrentStateName())
	{
		Get_Component<CAnimator3D>()->Set_Animation(1, "ThugAssaulter_Ani_Hit_Knock")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::Linear)
			.Loop(false)
			.Apply();

		m_tStatus.iNowHP -= fDamage * 1.2f; // 1.5f
	}
	else if ("Idle" == m_pStateMachine->Get_CurrentStateName())
	{
		m_pStateMachine->Set_Trigger("Idle_To_Hit");
		DIR eDir = GetDIRToPlayer(); 
		m_pStateMachine->Set_Int("Dir", ENUM(eDir));
	}
	else
	{
		Get_Component<CAnimator3D>()->Set_Animation(1, "ThugAssaulter_Ani_Hit_Stay")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::Linear)
			.Loop(false)
			.Apply();

		m_tStatus.iNowHP -= fDamage * 0.7f;
		m_tStatus.iGroggyValue += 4;
	}

	if (0.f > m_tStatus.iNowHP)
		m_tStatus.iNowHP = 0.f;
}

/* For.State Machine */
HRESULT CThugAssaulter::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CThugAssaulter>::Create();
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

HRESULT CThugAssaulter::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CThugAssaulter_Born::Create());
	m_pStateMachine->Register_State("Idle", CThugAssaulter_Idle::Create());
	m_pStateMachine->Register_State("Attack", CThugAssaulter_Attack::Create());
	m_pStateMachine->Register_State("Move", CThugAssaulter_Move::Create());
	m_pStateMachine->Register_State("Chase", CThugAssaulter_Chase::Create());
	m_pStateMachine->Register_State("Death", CThugAssaulter_Death::Create());
	m_pStateMachine->Register_State("Groggy", CThugAssaulter_Groggy::Create());
	m_pStateMachine->Register_State("Hit", CThugAssaulter_Hit::Create());
	m_pStateMachine->Register_State("Parried", CThugAssaulter_Parried::Create());

	return S_OK;
}

HRESULT CThugAssaulter::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CThugAssaulter>::CONDITION_ANIMATION_END);
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CThugAssaulter>::CONDITION_TRIGGER, "Idle_To_Attack");
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CThugAssaulter>::CONDITION_TRIGGER, "Idle_To_Move");
	m_pStateMachine->Register_Transition("Idle", "Chase",
		CStateMachine<CThugAssaulter>::CONDITION_TRIGGER, "Idle_To_Chase");
	m_pStateMachine->Register_Transition("Idle", "Death",
		CStateMachine<CThugAssaulter>::CONDITION_TRIGGER, "Idle_To_Death");
	m_pStateMachine->Register_Transition("Idle", "Groggy",
		CStateMachine<CThugAssaulter>::CONDITION_TRIGGER, "Idle_To_Groggy");
	m_pStateMachine->Register_Transition("Idle", "Hit",
		CStateMachine<CThugAssaulter>::CONDITION_TRIGGER, "Idle_To_Hit");
	
	return S_OK;
}

HRESULT CThugAssaulter::Ready_Rules()
{
	// x = Idle에서 다음 상태로 넘어가는 쿨타임, y = dt 더한 타이머용
	m_vIdleTime = { 1.f, 0.f };

	//// Target 감지 범위 (default = 5.f)
	//m_fDetectedRange = 5.f;

	m_tHysteriesis.fEvadeEnter = 2.f;
	m_tHysteriesis.fComboEnter = 3.f;
	m_tHysteriesis.fComboExit = 4.f;
	m_tHysteriesis.fChaseEnter = 7.f;
	m_tHysteriesis.fChaseExit = 5.f;

	return S_OK;
}

void CThugAssaulter::Update_States(_float dt)
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

void CThugAssaulter::ControlState(const _float dt)
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

void CThugAssaulter::CheckDistanceFromPlayer()
{
	if ("Chase" != m_pStateMachine->Get_CurrentStateName() &&
		m_tTargetingInfo.fDistance >= m_tHysteriesis.fChaseEnter)
		m_pStateMachine->Set_Bool("Chase", true);


	if (true == m_pStateMachine->Get_Bool("Chase") &&
		m_tTargetingInfo.fDistance <= m_tHysteriesis.fChaseExit)
		m_pStateMachine->Set_Bool("Chase", false);
}
 
