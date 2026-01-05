#include "pch.h"
#include "ThugBulkyEnforcer.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Component */
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "CharacterController.h"

/* States */
#include "StateMachine.h"
#include "ThugBulkyEnforcer_Idle.h"
#include "ThugBulkyEnforcer_Born.h"
#include "ThugBulkyEnforcer_Attack.h"
#include "ThugBulkyEnforcer_Move.h"



CThugBulkyEnforcer::CThugBulkyEnforcer()
	: CEnemy()
{
}

CThugBulkyEnforcer::CThugBulkyEnforcer(const CThugBulkyEnforcer& rhg)
	: CEnemy(rhg)
{
}

HRESULT CThugBulkyEnforcer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("Monster_ThugBulkyEnforcer.mat", "../Bin/Resources/Model/skeletal/Enemy/ThugBulkyEnforcer/Monster_ThugBulkyEnforcer.mat");
	pResourceMgr->Add_ResourcePath("Monster_ThugBulkyEnforcer.model", "../Bin/Resources/Model/skeletal/Enemy/ThugBulkyEnforcer/Monster_ThugBulkyEnforcer.model");
	pResourceMgr->Add_ResourcePath("ThugBulkyEnforcer_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/ThugBulkyEnforcer/ThugBulkyEnforcer_Meta.json");

	return S_OK;
}

HRESULT CThugBulkyEnforcer::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Monster_ThugBulkyEnforcer.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Monster_ThugBulkyEnforcer.mat");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Monster_ThugBulkyEnforcer.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "ThugBulkyEnforcer_Meta.json");
	pAnimator->Set_MotionBone(3);	//Bip001
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	// 임시 확인용
	CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;

	return S_OK;
}

void CThugBulkyEnforcer::Awake()
{
}

void CThugBulkyEnforcer::Priority_Update(_float dt)
{
}

void CThugBulkyEnforcer::Update(_float dt)
{

	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);

	__super::Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CThugBulkyEnforcer::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CThugBulkyEnforcer::Render_GUI()
{
	ImGui::PushID(this);

	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

	// =======================================================
	if (ImGui::TreeNode("Inspector##ThugBulkyInspector")) {
		__super::Render_GUI();
		ImGui::TreePop();
	}
	// =======================================================
#pragma region State
	ImGui::SeparatorText("State & BlackBoard");
	ImGui::BeginChild("State##ThugBulkyEnforcerStatus", ImVec2{ 0, childHeight + textLineHeight * 8}, true);
	string TagCurState = "Current State : " + m_pStateMachine->Get_CurrentStateName();
	ImGui::Text(TagCurState.c_str());
	string TagCurChildState = "Current ChildState : " + m_tAttackBlackBoard.currentStateTag;
	ImGui::Text(TagCurChildState.c_str());
	string TagStateQueueSize = "StateQueue Size : " + to_string(m_tAttackBlackBoard.stateQueue.size());
	ImGui::Text(TagStateQueueSize.c_str());

	// bool 변수 확인용(수정 불가)
	ImGui::BeginDisabled(true);
	ImGui::Checkbox(u8"IsRequestNext (다음 상태 존재)", &m_tAttackBlackBoard.isRequestNext);
	ImGui::Checkbox(u8"IsChainOpen (현재 상태 종료)", &m_tAttackBlackBoard.isChainOpen);
	ImGui::Checkbox(u8"isEnd (Queue에 등록된 마지막 상태 여부)", &m_tAttackBlackBoard.isEnd);
	ImGui::EndDisabled();
	ImGui::SeparatorText("Reseve State List");
	for (size_t i = 0; i < m_tAttackBlackBoard.stateQueue.size(); i++)
	{
		if (i == 0)
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), m_tAttackBlackBoard.stateQueue[i].c_str());
		else
			ImGui::Text(m_tAttackBlackBoard.stateQueue[i].c_str());

	}
	ImGui::EndChild();
#pragma endregion
	// =======================================================
	//ImGui::SeparatorText("For Target Information");
	//auto pCharacter = GetCharacterOnField();
	//if (nullptr != pCharacter) {
	//	ImGui::BeginChild("TracePlayer##ThugBulkyEnforcerTracePlayer", ImVec2{ 0, childHeight + textLineHeight * 6 }, true);
	//
	//	ImGui::Text("Character Name : %s", pCharacter->TagInstanceName);
	//	ImGui::Text("Character Pos : %.2f, %.2f, %.2f", pCharacter->vPos.x, pCharacter->vPos.y, pCharacter->vPos.z);
	//	ImGui::Text("Character CCT Radius : %.2f", pCharacter->fRadius);;
	//	ImGui::Text("Distance From Character : %.3f", m_tTargetingInfo.fDistance);
	//	ImGui::Text("Dot with Target : %.2f", m_tTargetingInfo.fDotTarget);
	//	ImGui::Text("Dir To Target : %.2f, %.2f, %.2f", m_tTargetingInfo.vDirToTarget.x, m_tTargetingInfo.vDirToTarget.y, m_tTargetingInfo.vDirToTarget.z);
	//	ImGui::BeginDisabled(true);
	//	ImGui::Checkbox(u8"isDetected (플레이어 감지)", &m_tTargetingInfo.isDetected);
	//	ImGui::EndDisabled();
	//
	//ImGui::EndChild();
	//}
	Render_GUI_ForTargetInfo();
	// =======================================================
	
	ImGui::Checkbox("Auto Pattern", &m_isAutoPatternPlay);

	// =======================================================
	if(ImGui::TreeNode("Test State##ThugBulkyEnforcerTestState")) {
		ImGui::BeginChild("State##ThugBulkyEnforcerStatus", ImVec2{ 0, childHeight }, true);

		if (ImGui::Button(u8"1.오른쪽 위빙 후 오른쪽 훅")) {
			m_pStateMachine->Set_Int("AttackPattern", 1);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"2.오른손 어퍼 후 왼손 내려찍기")) {
			m_pStateMachine->Set_Int("AttackPattern", 2);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"3.플라잉 니킥 후 양손 내려찍으며 착지")) {
			m_pStateMachine->Set_Int("AttackPattern", 3);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"4.크게 움직이며 양손으로 바닥 내려찍기")) {
			m_pStateMachine->Set_Int("AttackPattern", 4);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"5.오른쪽 위빙 후 왼쪽 위빙하면서 왼손 훅")) {
			m_pStateMachine->Set_Int("AttackPattern", 5);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"6.왼쪽 위빙 후 오른손 어퍼 + 왼손 내려찍기")) {
			m_pStateMachine->Set_Int("AttackPattern", 6);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}


		ImGui::EndChild();
		ImGui::TreePop();
	}


	ImGui::PopID();
}

CThugBulkyEnforcer* CThugBulkyEnforcer::Create()
{
	CThugBulkyEnforcer* instance = new CThugBulkyEnforcer();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CThugBulkyEnforcer");
	}

	return instance;
}

CGameObject* CThugBulkyEnforcer::Clone(INIT_DESC* pArg)
{
	CThugBulkyEnforcer* instance = new CThugBulkyEnforcer(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrificeHand");
	}

	return instance;
}

void CThugBulkyEnforcer::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

/* For.State Machine */
HRESULT CThugBulkyEnforcer::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();
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

	Get_Component<CAnimator3D>()->Set_Animation("ThugBulkyEnforcer_Ani_Born")
		.Apply();

	return S_OK;
}

HRESULT CThugBulkyEnforcer::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CThugBulkyEnforcer_Born::Create());
	m_pStateMachine->Register_State("Idle", CThugBulkyEnforcer_Idle::Create());
	m_pStateMachine->Register_State("Attack", CThugBulkyEnforcer_Attack::Create());
	m_pStateMachine->Register_State("Move", CThugBulkyEnforcer_Move::Create());


	return S_OK;
}

HRESULT CThugBulkyEnforcer::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_ANIMATION_END);

	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_TRIGGER, "Idle_To_Attack");

	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_TRIGGER, "Idle_To_Move");

	return S_OK;
}

HRESULT CThugBulkyEnforcer::Ready_Rules()
{
	// x = Idle에서 다음 상태로 넘어가는 쿨타임, y = dt 더한 타이머용
	m_vIdleTime = { 0.2f, 0.f };

	// Target 감지 범위 (default = 5.f)
	m_fDetectedRange = 10.f;

	return S_OK;
}

void CThugBulkyEnforcer::Update_States(_float dt)
{
	if (true == m_isIdle) {
		m_pStateMachine->Change_State("Idle");
		m_pStateMachine->Reset_Trigger("Idle_To_Attack");
		m_pStateMachine->Reset_Trigger("Idle_To_Move");

		m_isIdle = false;
	}

	CheckDistanceFromPlayer();

	if (true == m_isAutoPatternPlay &&
		"Idle" == m_pStateMachine->Get_CurrentStateName()) {
		
		m_vIdleTime.y += dt;

		if (m_vIdleTime.x <= m_vIdleTime.y) {
			if (true == m_pStateMachine->Get_Bool("FinishAttack"))
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			else
				m_pStateMachine->Set_Trigger("Idle_To_Attack");

			m_vIdleTime.y = 0.f;
		}

		
	}
}

void CThugBulkyEnforcer::CheckDistanceFromPlayer()
{
	if (true == m_tTargetingInfo.isDetected)
		m_pStateMachine->Set_Bool("Chase", true);
}
