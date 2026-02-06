#include "pch.h"
#include "Cyclops.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Child */
#include "AttackSign.h"
#include "Cyclops_Spit.h"

/* Component */
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ObjectContainer.h"
#include "CharacterController.h"
#include "BoneFollower.h"

/* States */
#include "StateMachine.h"
#include "Cyclops_Attack.h"
#include "Cyclops_Born.h"
#include "Cyclops_Death.h"
#include "Cyclops_Groggy.h"
#include "Cyclops_Hit.h"
#include "Cyclops_Idle.h"
#include "Cyclops_Move.h"
#include "Cyclops_Chase.h"

CCyclops::CCyclops()
	: CEnemyNormal()
{
}

CCyclops::CCyclops(const CCyclops& rhg)
	: CEnemyNormal(rhg)
{
}

HRESULT CCyclops::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("Cyclops.mat", "../Bin/Resources/Zero/Enemy/Cyclops/Cyclops.mat");
	pResourceMgr->Add_ResourcePath("Cyclops.model", "../Bin/Resources/Zero/Enemy/Cyclops/Cyclops.model");
	//pResourceMgr->Add_ResourcePath("Monster_Cyclops_Meta.json", "../Bin/Resources/Zero/Enemy/Cyclops/Monster_Cyclops_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Cyclops.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Cyclops.mat");

	return S_OK;
}

HRESULT CCyclops::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Cyclops.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_Cyclops_Meta.json");
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	pAnimator->Resize_Layer(2);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);

	if (FAILED(Ready_Children(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CCyclops::Awake()
{
	__super::Awake();
}

void CCyclops::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CCyclops::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);

	__super::Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CCyclops::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);

	__super::Late_Update(dt);
}

void CCyclops::Render_GUI()
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

			if (ImGui::Button("Parried"))
				Parried();

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Spit##Spit"))
		{
			// 침 확인용
			ImGui::BeginDisabled(true);
			auto pObjectContainerCom = Get_Component<CObjectContainer>();
			for (auto& index : m_SpitIndex)
			{
				auto pSpit = dynamic_cast<CCyclops_Spit*> (pObjectContainerCom->Get_ChildByOrder(index));
				if (nullptr == pSpit)
					continue;
				_bool isAlive = pSpit->Is_Alive();
				ImGui::Checkbox(pSpit->Get_InstanceName().c_str(), &isAlive);
			}
			ImGui::EndDisabled();

			if (ImGui::Button("Spit Straight"))
				Spit(ENUM(CCyclops_Spit::SPIT::STRAIGHT));
			if (ImGui::Button("Spit Arc Center"))
				Spit(ENUM(CCyclops_Spit::SPIT::ARC_CENTER));
			if (ImGui::Button("Spit Arc Left"))
				Spit(ENUM(CCyclops_Spit::SPIT::ARC_LEFT));
			if (ImGui::Button("Spit Arc Riight"))
				Spit(ENUM(CCyclops_Spit::SPIT::ARC_RIGHT));
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
#pragma endregion

#pragma region AutoPattern
	ImGui::Checkbox("Auto Pattern", &m_isAutoPatternPlay);
#pragma endregion

	if (ImGui::Button("Execute"))
		m_tStatus.iNowHP -= m_tStatus.iMaxHP;

	if (ImGui::Button("Groggy"))
		m_tStatus.iGroggyValue += 100;

	if (ImGui::Button("Test Alive"))
		Set_Alive(!Is_Alive());


	ImGui::PopID();
}

void CCyclops::OnPooledAcquire(INIT_DESC* pArg)
{
	Initialize(pArg);
}

void CCyclops::OnPooledRelease()
{
}

void CCyclops::Parried()
{
	if ("Attack" != m_pStateMachine->Get_CurrentStateName())
		return;

	__super::Parried();

	m_pStateMachine->Change_State("Parried");
	SetOnAttack(false, ATTACK_SIDE::NONE); 
}

HRESULT CCyclops::Ready_Children(INIT_DESC* pArg)
{
	BATTLE_COLLIDER_DESC WeaponDesc = {};

	WeaponDesc.tagName = "Head";
	WeaponDesc.isAttachBone = true;
	WeaponDesc.tagBone = "Bn_Head";
	WeaponDesc.pOwnerAnimator3D = Get_Component<CAnimator3D>();
	WeaponDesc.vAttackSize = { 0.5f,0.f,0.f };

	if (FAILED(AttachBattleColliderObject(&WeaponDesc)))
		return E_FAIL;

	Create_AttackSign("Bn_Head");
	Create_UIEnemyStatus("Bip001_Spine");
	Create_MeshPyramid();

	Ready_Spit(3);

	return S_OK;
}

HRESULT CCyclops::Ready_Spit(_uint iNum)
{
	if (1 > iNum)
		return E_FAIL;

	string tagNowLevel = LevelManager()->Get_NowLevelKey();
	auto pObjectContainer = Get_Component<CObjectContainer>();

	_float4x4* pHeadBone = Get_Component<CAnimator3D>()->Get_BoneMatrixPtr(CAnimator3D::BoneSpace::COMBINED, "Bn_Head");
	if (nullptr == pHeadBone)
		return E_FAIL;
	for (_uint i = 0; i < iNum; ++i)
	{
		string tagInstanceName = "Spit" + to_string(i);

		CCyclops_Spit::SPIT_DESC* pDesc = new CCyclops_Spit::SPIT_DESC();
		pDesc->pHeadBone = pHeadBone;

		COLLIDER_DESC SpitColDesc = {};
		SpitColDesc.eGroup = COLLISION_GROUP::MONSTER;
		SpitColDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
		SpitColDesc.bTrigger = true;
		SpitColDesc.bAutoFit = false;
		SpitColDesc.eType = COLLIDER_TYPE::SPHERE;
		SpitColDesc.vSize = { 0.2f, 0.f, 0.f };

		auto pSpit = Builder::Create_Object({ tagNowLevel , "Proto_GameObject_Cyclops_Spit" })
			.Add_ObjDesc(pDesc)
			.Collider(SpitColDesc)
			.Build(tagInstanceName);

		if (nullptr == pSpit)
			continue;

		_int iChildIndex = pObjectContainer->Add_Child(pSpit, false);

		m_SpitIndex.push_back(iChildIndex);
	}

	return S_OK;
}


CCyclops* CCyclops::Create()
{
	CCyclops* instance = new CCyclops();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CCyclops");
	}

	return instance;
}

CGameObject* CCyclops::Clone(INIT_DESC* pArg)
{
	CCyclops* instance = new CCyclops(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CCyclops");
	}

	return instance;
}

void CCyclops::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}


void CCyclops::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	__super::TakeDamage(eDamageType, fDamage, charaName);

	if (0 >= m_tStatus.iNowHP)
		return;

	if ("Groggy" == m_pStateMachine->Get_CurrentStateName())
	{
		Get_Component<CAnimator3D>()->Set_Animation(1, "Cyclops_Ani_Hit_Knock")
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
		Get_Component<CAnimator3D>()->Set_Animation(1, "Cyclops_Ani_Hit_Stay")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::Linear)
			.Loop(false)
			.Apply();
	}
}

void CCyclops::Spit(_uint iSpitType)
{
	if (ENUM(CCyclops_Spit::SPIT::ARC_RIGHT) < iSpitType || 0 > iSpitType)
		return;

	auto pObjectContainerCom = Get_Component<CObjectContainer>();
	for (auto& index : m_SpitIndex)
	{
		auto pSpit = dynamic_cast<CCyclops_Spit*> (pObjectContainerCom->Get_ChildByOrder(index));
		if (nullptr == pSpit)
			continue;

		if (false == pSpit->Is_Alive())
		{
			pSpit->ShootSpit(static_cast<CCyclops_Spit::SPIT>(iSpitType));
			return;
		}
	}


}

/* For.State Machine */
HRESULT CCyclops::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CCyclops>::Create();
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

	Get_Component<CAnimator3D>()->Set_Animation("Monster_Cyclops_Ani_Born")
		.Apply();

	return S_OK;
}

HRESULT CCyclops::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CCyclops_Born::Create());
	m_pStateMachine->Register_State("Idle", CCyclops_Idle::Create());
	m_pStateMachine->Register_State("Attack", CCyclops_Attack::Create());
	m_pStateMachine->Register_State("Move", CCyclops_Move::Create());
	m_pStateMachine->Register_State("Chase", CCyclops_Chase::Create());
	m_pStateMachine->Register_State("Death", CCyclops_Death::Create());
	m_pStateMachine->Register_State("Groggy", CCyclops_Groggy::Create());
	m_pStateMachine->Register_State("Hit", CCyclops_Hit::Create());

	return S_OK;
}

HRESULT CCyclops::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CCyclops>::CONDITION_ANIMATION_END);
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CCyclops>::CONDITION_TRIGGER, "Idle_To_Attack");
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CCyclops>::CONDITION_TRIGGER, "Idle_To_Move");
	m_pStateMachine->Register_Transition("Idle", "Chase",
		CStateMachine<CCyclops>::CONDITION_TRIGGER, "Idle_To_Chase");
	m_pStateMachine->Register_Transition("Idle", "Death",
		CStateMachine<CCyclops>::CONDITION_TRIGGER, "Idle_To_Death");
	m_pStateMachine->Register_Transition("Idle", "Groggy",
		CStateMachine<CCyclops>::CONDITION_TRIGGER, "Idle_To_Groggy");
	m_pStateMachine->Register_Transition("Idle", "Hit",
		CStateMachine<CCyclops>::CONDITION_TRIGGER, "Idle_To_Hit");

	return S_OK;
}

HRESULT CCyclops::Ready_Rules()
{
	// x = Idle에서 다음 상태로 넘어가는 쿨타임, y = dt 더한 타이머용
	m_vIdleTime = { 1.f, 0.f };

	m_tHysteriesis.fEvadeEnter = 3.f;
	m_tHysteriesis.fComboEnter = 5.0f;
	m_tHysteriesis.fComboExit = 8.f;
	m_tHysteriesis.fChaseEnter = 12.f;
	m_tHysteriesis.fChaseExit = 10.f;

	return S_OK;
}

void CCyclops::Update_States(_float dt)
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

void CCyclops::ControlState(const _float dt)	
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

void CCyclops::CheckDistanceFromPlayer()
{
	if ("Chase" != m_pStateMachine->Get_CurrentStateName() &&
		m_tTargetingInfo.fDistance >= m_tHysteriesis.fChaseEnter)
		m_pStateMachine->Set_Bool("Chase", true);

	if (true == m_pStateMachine->Get_Bool("Chase") &&
		m_tTargetingInfo.fDistance <= m_tHysteriesis.fChaseExit)
		m_pStateMachine->Set_Bool("Chase", false);
} 