#include "pch.h"
#include "ThugBulkyEnforcer.h"

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
#include "ThugBulkyEnforcer_Idle.h"
#include "ThugBulkyEnforcer_Born.h"
#include "ThugBulkyEnforcer_Attack.h"
#include "ThugBulkyEnforcer_Move.h"
#include "ThugBulkyEnforcer_Chase.h"
#include "ThugBulkyEnforcer_Groggy.h"
#include "ThugBulkyEnforcer_Death.h"

#include "ThugBulkyEnforcer_Collider.h"

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
	Add_Component<CObjectContainer>();
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
	pAnimator->Resize_Layer(2);
	for (size_t i = 1; i < 2; i++)
		pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, i);

	if (FAILED(Ready_Children(pArg)))
		return E_FAIL;

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
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CThugBulkyEnforcer::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);

	__super::Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CThugBulkyEnforcer::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
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
	ImGui::BeginChild("##ThugBulkyEnforcerStatus", ImVec2{ 0, childHeight + textLineHeight * 6}, true);
	//string TagCurState = "Current State : " + m_pStateMachine->Get_CurrentStateName();
	ImGui::Text("Current State : %s", m_pStateMachine->Get_CurrentStateName().c_str());
	//string TagCurChildState = "Current ChildState : " + m_tAttackBlackBoard.currentStateTag;
	ImGui::Text("Current ChildState : %s", m_tAttackBlackBoard.currentStateTag.c_str());
	//string TagStateQueueSize = "StateQueue Size : " + to_string(m_tAttackBlackBoard.stateQueue.size());
	//ImGui::Text("StateQueue Size : %d", (_int)m_tAttackBlackBoard.stateQueue.size());
	ImGui::Text("AttackCombo : %d", m_iAttackCombo);

	// bool 변수 확인용(수정 불가)
	ImGui::BeginDisabled(true);
	ImGui::Checkbox(u8"IsRequestNext (다음 상태 존재)", &m_tAttackBlackBoard.isRequestNext);
	ImGui::Checkbox(u8"IsChainOpen (현재 상태 종료)", &m_tAttackBlackBoard.isChainOpen);
	ImGui::Checkbox(u8"isEnd (Queue에 등록된 마지막 상태 여부)", &m_tAttackBlackBoard.isEnd);
	ImGui::EndDisabled();
	ImGui::SeparatorText("Reseve State List");
	for (size_t i = 0; i < m_AttackHistory.size(); i++)
	{

		if (i == 0)
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "%d", m_AttackHistory[i]);
		else
			ImGui::Text("%d", m_AttackHistory[i]);
	}
	
	//for (size_t i = 0; i < m_tAttackBlackBoard.stateQueue.size(); i++)
	//{
	//	if (i == 0)
	//		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), m_tAttackBlackBoard.stateQueue[i].c_str());
	//	else
	//		ImGui::Text(m_tAttackBlackBoard.stateQueue[i].c_str());
	//
	//}
	ImGui::EndChild();
#pragma endregion
	// =======================================================
	ImGui::SeparatorText("Status");
	auto pCharacter = GetCharacterOnField();
	if (nullptr != pCharacter) {
		ImGui::BeginChild("TracePlayer##ThugBulkyEnforcerStatus", ImVec2{ 0, childHeight + textLineHeight * 3 }, true);
		
		ImGui::Text("AnimName : %s", Get_Component<CAnimator3D>()->Get_CurAnimName().c_str());
		ImGui::Text("SelfDir: %.2f, %.2f, %.2f", m_tTargetingInfo.vDirSelfLook.x, m_tTargetingInfo.vDirSelfLook.y, m_tTargetingInfo.vDirSelfLook.z);
		ImGui::Text("CaptureDir: %.2f, %.2f, %.2f", m_vDirToLookCapture.x, m_vDirToLookCapture.y, m_vDirToLookCapture.z);
		ImGui::Text("Groggy Value : %d", m_iGroggyValue);

		ImGui::BeginDisabled(true);
		ImGui::Checkbox(u8"isLookPlayer", &m_isLookPlayer);
		ImGui::EndDisabled();
	
	ImGui::EndChild();
	}
	Render_GUI_ForTargetInfo();
	// =======================================================
	
	ImGui::Checkbox("Auto Pattern", &m_isAutoPatternPlay);

	// =======================================================
	if(ImGui::TreeNode("Test State##ThugBulkyEnforcerTestState")) {
		//ImGui::BeginChild("State##ThugBulkyEnforcerStatus", ImVec2{ 0, childHeight }, true);

		if (ImGui::TreeNode("AttackState##ThugBulkyEnforcerTestState_Attack")) {
			if (ImGui::Button(u8"1.오른쪽 훅")) {
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
			if (ImGui::Button(u8"5.왼쪽 위빙하면서 왼손 훅")) {
				m_pStateMachine->Set_Int("AttackPattern", 5);
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}
			if (ImGui::Button(u8"6.오른손 어퍼 + 왼손 내려찍기")) {
				m_pStateMachine->Set_Int("AttackPattern", 6);
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("MoveState##ThugBulkyEnforcerTestState_Move")) {

			if (ImGui::Button(u8"1.Walk_Front")) {
				m_pStateMachine->Set_Int("MovePattern", 1);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"2.Walk_Back")) {
				m_pStateMachine->Set_Int("MovePattern", 2);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"3.Walk_Left")) {
				m_pStateMachine->Set_Int("MovePattern", 3);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"4.Walk_Right")) {
				m_pStateMachine->Set_Int("MovePattern", 4);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"5.Walk_RF_LFoot")) {
				m_pStateMachine->Set_Int("MovePattern", 5);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"6.Walk_FR_RFoot")) {
				m_pStateMachine->Set_Int("MovePattern", 6);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"7.Walk_LF_RFoot")) {
				m_pStateMachine->Set_Int("MovePattern", 7);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"8.SideStep_L")) {
				m_pStateMachine->Set_Int("MovePattern", 8);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"8.SideStep_L(Shortly)")) {
				m_pStateMachine->Set_Int("MovePattern", 8);
				m_pStateMachine->Set_Bool("ShortlyMove", true);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"9.SideStep_R")) {
				m_pStateMachine->Set_Int("MovePattern", 9);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"9.SideStep_R(Shortly)")) {
				m_pStateMachine->Set_Int("MovePattern", 9);
				m_pStateMachine->Set_Bool("ShortlyMove", true);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			if (ImGui::Button(u8"10.Evade")) {
				m_pStateMachine->Set_Int("MovePattern", 10);
				m_pStateMachine->Set_Trigger("Idle_To_Move");
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Hit & Groggy##ThugBulkyEnforcerTestHitAndGroggy")) {
			
			if (ImGui::Button("Increase Groggy value 30"))
				m_iGroggyValue += 30;
			if (ImGui::Button("Hit")) {
				if ("Groggy" == m_pStateMachine->Get_CurrentStateName()) {
					//for (size_t i = 9; i < 10; i++)
					//{
						Get_Component<CAnimator3D>()->Set_Animation(1, "ThugBulkyEnforcer_Ani_Stun_Hit_Stay")
							.LayerBlend(1.f, 0.5f, 1.f, EaseType::Linear)
							.Loop(false)
							.Apply();
					//}
				}
				else {
					//for (size_t i = 1; i < 10; i++)
					//{
						Get_Component<CAnimator3D>()->Set_Animation(1, "ThugBulkyEnforcer_Ani_Hit_Shake")
							.LayerBlend(1.f, 0.5f, 1.f, EaseType::Linear)
							.Loop(false)
							.Apply();
					//}
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Death##ThugBulkyEnforcerTestDeath")) {
			if (ImGui::Button("Death Front")) 
				m_pStateMachine->Change_State("Death");
			if (ImGui::Button("Death Back")) {
				m_pStateMachine->Set_Bool("DeathBack", true);
				m_pStateMachine->Change_State("Death");
			}
			
			ImGui::TreePop();
		}

		//ImGui::EndChild();
		ImGui::TreePop();
	}

	if (ImGui::Button("Open StateMachine"))
		m_pStateMachine->Set_ShowWindow(true);
	m_pStateMachine->Render_GUI();
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
		MSG_BOX("Failed to clone : CThugBulkyEnforcer");
	}

	return instance;
}

void CThugBulkyEnforcer::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

HRESULT CThugBulkyEnforcer::Ready_Children(INIT_DESC* pArg)
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	RIGIDBODY_DESC rigidbodyDesc = {};
	rigidbodyDesc.bEnableGravity = false;
	rigidbodyDesc.isKinematic = true;
	rigidbodyDesc.bLockY = true;
	
	COLLIDER_DESC colliderDesc = {};
	colliderDesc.eType = COLLIDER_TYPE::SPHERE;
	colliderDesc.eGroup = COLLISION_GROUP::MONSTER_ATTACK;
	//colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);

	colliderDesc.vSize = { 0.3f,0.f,0.f };
	colliderDesc.bAutoFit = false;

	auto pWeapon_L = Builder::Create_Object({ "Test_Level", "Proto_GameObject_ThugBulkyEnforcer_Collider" })
		.RigidBody(rigidbodyDesc)
		.Collider(colliderDesc)
		.Build("ThugBulkyEnforcer_Weapon_Left");

	pObjectContainer->Add_Child(pWeapon_L, false);

	pWeapon_L->Get_Component<CBoneFollower>()->Link_Bone(Get_Component<CAnimator3D>(), "Ctr_L_Weapon_3");

	auto pWeapon_R = Builder::Create_Object({ "Test_Level", "Proto_GameObject_ThugBulkyEnforcer_Collider" })
		.RigidBody(rigidbodyDesc)
		.Collider(colliderDesc)
		.Build("ThugBulkyEnforcer_Weapon_Right");

	pObjectContainer->Add_Child(pWeapon_R, false);

	pWeapon_R->Get_Component<CBoneFollower>()->Link_Bone(Get_Component<CAnimator3D>(), "Ctr_R_Weapon_3");




	//"Ctr_R_Weapon_3"
	//"Ctr_L_Weapon_3"


	return S_OK;
}

void CThugBulkyEnforcer::CaptureRotateDir(_float3 vTargetDir, _float fSpeed)
{
	m_isLookPlayer = true;
	m_vDirToLookCapture = vTargetDir;
	m_fRotateSpeed = fSpeed;
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
	m_pStateMachine->Register_State("Chase", CThugBulkyEnforcer_Chase::Create());
	m_pStateMachine->Register_State("Groggy", CThugBulkyEnforcer_Groggy::Create());
	m_pStateMachine->Register_State("Death", CThugBulkyEnforcer_Death::Create());

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

	m_pStateMachine->Register_Transition("Idle", "Chase",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_TRIGGER, "Idle_To_Chase");

	m_pStateMachine->Register_Transition("Idle", "Groggy",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_TRIGGER, "Idle_To_Groggy");

	m_pStateMachine->Register_Transition("Idle", "Death",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_TRIGGER, "Idle_To_Death");

	return S_OK;
}

HRESULT CThugBulkyEnforcer::Ready_Rules()
{
	// x = Idle에서 다음 상태로 넘어가는 쿨타임, y = dt 더한 타이머용
	m_vIdleTime = { 0.2f, 0.f };

	// Target 감지 범위 (default = 5.f)
	m_fDetectedRange = 5.f;

	m_tHysteriesis.fEvadEnter = 2.f;
	m_tHysteriesis.fComboEnter = 3.f;
	m_tHysteriesis.fComboExit = 4.f;
	m_tHysteriesis.fChaseEnter = 7.f;
	m_tHysteriesis.fChaseExit = 5.f;

	m_pStateMachine->Set_Int("MovePattern", -1);

	return S_OK;
}

void CThugBulkyEnforcer::Update_States(_float dt)
{
	if (true == m_isIdle) {
		m_pStateMachine->Change_State("Idle");
		m_pStateMachine->Reset_Trigger("Idle_To_Attack");
		m_pStateMachine->Reset_Trigger("Idle_To_Move");
		m_pStateMachine->Reset_Trigger("Idle_To_Chase");
		m_pStateMachine->Reset_Trigger("Idle_To_Groggy");
		m_pStateMachine->Reset_Trigger("Idle_To_Death");

		m_isIdle = false;
	}

	ManageGroggy(dt);
	ManageAttackHistory();
	CheckDistanceFromPlayer();
	RotateToPlayer(dt);
	
	//================================
	ControlState(dt);
	//================================
}

void CThugBulkyEnforcer::ControlState(const _float dt)
{
	if (100 == m_iGroggyValue) {
		m_pStateMachine->Change_State("Groggy");
	}

	if (true == m_isAutoPatternPlay &&
		"Idle" == m_pStateMachine->Get_CurrentStateName()) {

		m_vIdleTime.y += dt;

		if (m_vIdleTime.x <= m_vIdleTime.y) {

			const _float fDistanceToPlayer = m_tTargetingInfo.fDistance;

			if (true == m_pStateMachine->Get_Bool("Chase")) {
				if (2 == m_iAttackCombo)
					m_iAttackCombo = 0;
				m_pStateMachine->Set_Trigger("Idle_To_Chase");
			}
			else if ("Attack" == m_pStateMachine->Get_PrevStateName()) {
				
				// 콤보 사이에 위빙 할지 말지 정하기. 안하면 공격으로 바로 이동
				if (3 > m_iAttackCombo) {
					_int iSidestepIndex = Helper::Get_Random_Int(0, 3);
					switch (iSidestepIndex)
					{
					case 0:
					case 1:
					{
						m_pStateMachine->Set_Bool("Sidestep", true);
						m_pStateMachine->Set_Bool("ShortlyMove", true);
						m_pStateMachine->Set_Trigger("Idle_To_Move");
						break;
					}
					case 2:
					case 3:
					{
						if (m_tTargetingInfo.fDistance <= m_tHysteriesis.fEvadEnter) {
							m_pStateMachine->Set_Bool("Evade", true);
							m_pStateMachine->Set_Trigger("Idle_To_Move");
						}
						else {
							m_pStateMachine->Set_Trigger("Idle_To_Attack");
							if (2 == m_iAttackCombo)
								m_tAttackBlackBoard.isEnd = true;
							m_iAttackCombo++;
						}
					}
					}

				}
				else {	// 콤보(3회)가 끝났을 때 설렁설렁 걸어다님(휴식)
					m_iAttackCombo = 0;
					m_pStateMachine->Set_Trigger("Idle_To_Move");
				}
			}
			else {		// 첫 딜사이클 들어갈때
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
				m_iAttackCombo++;
			}

			m_vIdleTime.y = 0.f;
		}
	}
}

void CThugBulkyEnforcer::CheckDistanceFromPlayer()
{
	if ("Chase" != m_pStateMachine->Get_CurrentStateName() &&
		m_tTargetingInfo.fDistance >= m_tHysteriesis.fChaseEnter)
		m_pStateMachine->Set_Bool("Chase", true);

	
	if (true == m_pStateMachine->Get_Bool("Chase") &&
		m_tTargetingInfo.fDistance <= m_tHysteriesis.fChaseExit)
		m_pStateMachine->Set_Bool("Chase", false);
}

void CThugBulkyEnforcer::RotateToPlayer(const _float dt)
{
	if (false == m_isLookPlayer)
		return;

	_vector vTargetDir = XMLoadFloat3(&m_vDirToLookCapture);
	_vector	vSelfDir = m_pTransform->Dir(Engine::STATE::LOOK);
	vTargetDir = XMVector3Normalize(vTargetDir);
	vSelfDir = XMVector3Normalize(vSelfDir);

	_float fDot = XMVectorGetX(XMVector3Dot(vSelfDir, vTargetDir));
	fDot = max(-1, min(1.f, fDot));
	_float fAngle = acosf(fDot);

	_float fCross = XMVectorGetY(XMVector3Cross(vSelfDir, vTargetDir));
	if (0 > fCross)
		fAngle = -fAngle;

	if (fDot > 0.99f) {
		m_isLookPlayer = false;
		return;
	}

	m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle) * m_fRotateSpeed);
}

void CThugBulkyEnforcer::ManageAttackHistory()
{
	_uint iSize = static_cast<_uint>(m_AttackHistory.size());
	if (5 <= iSize)
		m_AttackHistory.pop_back();
}

void CThugBulkyEnforcer::ManageGroggy(const _float dt)
{
	if (100 < m_iGroggyValue)
		m_iGroggyValue = 100;

	if ("Groggy" == m_pStateMachine->Get_CurrentStateName()) {
		m_fGroggyDecreaseTime += dt;

		if (0.1f <= m_fGroggyDecreaseTime) {
			--m_iGroggyValue;
			m_fGroggyDecreaseTime = 0.f;
		}

		if (0 > m_iGroggyValue)
			m_iGroggyValue = 0;
	}
}
