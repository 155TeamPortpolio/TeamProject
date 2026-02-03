#include "pch.h"
#include "ThugPoacher.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Child */
#include "AttackSign.h"
#include "ThugPoacher_Arrow.h"

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
	pResourceMgr->Add_ResourcePath("ThugPoacher.mat", "../Bin/Resources/Zero/Enemy/ThugPoacher/ThugPoacher.mat");
	pResourceMgr->Add_ResourcePath("ThugPoacher.model", "../Bin/Resources/Zero/Enemy/ThugPoacher/ThugPoacher.model");
	//pResourceMgr->Add_ResourcePath("ThugPoacher_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/ThugPoacher/ThugPoacher_Meta.json");
	
	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "ThugPoacher.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "ThugPoacher.mat");

	return S_OK;
}

HRESULT CThugPoacher::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

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

	if (ImGui::Button("ShootArrow##ShootArrow"))
	{
		ShootArrow();
	}


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
}

HRESULT CThugPoacher::Ready_Children(INIT_DESC* pArg)
{
	Create_AttackSign("Bip001_Head");
	Create_UIEnemyStatus("Bip001_Spine2");
	Create_MeshPyramid();
	
	Ready_Arrows(3);

	return S_OK;
}

HRESULT CThugPoacher::Ready_Arrows(_uint iNum)
{
	if (1 > iNum)
		return E_FAIL;

	string tagNowLevel = LevelManager()->Get_NowLevelKey();
	auto pObjectContainer = Get_Component<CObjectContainer>();

	_float4x4* pWeaponBone = Get_Component<CAnimator3D>()->Get_BoneMatrixPtr(CAnimator3D::BoneSpace::COMBINED, "CrossbowD_01");
	if (nullptr == pWeaponBone)
		return E_FAIL;
	for (_uint i = 0; i < iNum; ++i)
	{
		string tagInstanceName = "Arrow" + to_string(i);

		CThugPoacher_Arrow::ARROW_DESC* pDesc = new CThugPoacher_Arrow::ARROW_DESC();
		pDesc->pWeapon = pWeaponBone;

		COLLIDER_DESC ArrowColDesc = {};
		ArrowColDesc.eGroup = COLLISION_GROUP::MONSTER;
		ArrowColDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
		ArrowColDesc.bTrigger = true;
		ArrowColDesc.bAutoFit = false;
		ArrowColDesc.eType = COLLIDER_TYPE::SPHERE;
		ArrowColDesc.vSize = { 0.2f, 0.f, 0.f };

		auto pArrow = Builder::Create_Object({ tagNowLevel , "Proto_GameObject_ThugPoacher_Arrow" })
			.Add_ObjDesc(pDesc)
			.Collider(ArrowColDesc)
			.Build(tagInstanceName);

		if (nullptr == pArrow)
			continue;

		_int iChildIndex = pObjectContainer->Add_Child(pArrow, false);

		m_ArrowsChildIndices.push_back(iChildIndex);
	}

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


void CThugPoacher::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	__super::TakeDamage(eDamageType, fDamage, charaName);

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

void CThugPoacher::ShootArrow()
{
	auto pObjectContainerCom = Get_Component<CObjectContainer>();
	for (auto& index : m_ArrowsChildIndices)
	{
		auto pArrow = dynamic_cast<CThugPoacher_Arrow*>(pObjectContainerCom->Get_ChildByOrder(index));
		if (nullptr == pArrow)
			continue;

		if (false == pArrow->Is_Alive())
		{
			pArrow->ShootArrow();
			return;
		}
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

	m_tHysteriesis.fEvadeEnter = 3.f;
	m_tHysteriesis.fComboEnter = 3.5f;
	m_tHysteriesis.fComboExit = 4.5f;
	m_tHysteriesis.fChaseEnter = 7.f;
	m_tHysteriesis.fChaseExit = 5.f;

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

void CThugPoacher::CheckDistanceFromPlayer()
{
	if ("Chase" != m_pStateMachine->Get_CurrentStateName() &&
		m_tTargetingInfo.fDistance >= m_tHysteriesis.fChaseEnter)
		m_pStateMachine->Set_Bool("Chase", true);
	
	if (true == m_pStateMachine->Get_Bool("Chase") &&
		m_tTargetingInfo.fDistance <= m_tHysteriesis.fChaseExit)
		m_pStateMachine->Set_Bool("Chase", false);
}