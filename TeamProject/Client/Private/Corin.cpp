#include "pch.h"
#include "Corin.h"
#include "GameInstance.h"
#include "BattleSystem.h"

#include "Material.h"

#include "Animator3D.h"
#include "CharacterController.h"

#include "StateMachine.h"
#include "CorinState_Idle.h"
#include "CorinState_Move.h"
#include "CorinState_Attack.h"
#include "CorinState_NormalAttack.h"
#include "CorinState_Evade.h"
#include "CorinState_SwitchIn.h"
#include "CorinState_SwitchOut.h"
#include "CorinState_Hit.h"

#include "FootIK.h"

#include "DataBase.h"

CCorin::CCorin()
{
}

CCorin::CCorin(const CCorin& rhs)
	: CCharacter(rhs)
{
}

HRESULT CCorin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Corin.model",
		"../Bin/Resources/Model/skeletal/Corin/Corin.model");
	pRcsMgr->Add_ResourcePath("Corin.mat",
		"../Bin/Resources/Model/skeletal/Corin/Corin.mat");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size01_Corin_Meta.json",
		"../Bin/Resources/Model/skeletal/Corin/Avatar_Female_Size01_Corin_Meta.json");

	Get_Component<CModel>()->Link_Model(G_GlobalLevelKey, "Corin.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Corin.mat");
	return S_OK;
}

HRESULT CCorin::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;
	if (FAILED(Initialize_StateMachine())) return E_FAIL;
	if (FAILED(Initialize_Weapon()))	   return E_FAIL;

	return S_OK;
}

void CCorin::Awake()
{
	__super::Awake();

	m_pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Corin.model");
	m_pAnimator->Link_MetaData(G_GlobalLevelKey, "Avatar_Female_Size01_Corin_Meta.json");

	//m_pAnimator->Set_MotionBone(12);
	m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);

	//*name change*
	m_strAnimName = "Avatar_Female_Size01_Corin_Ani_";
	m_strName = "Corin";
	m_eCharacterName = CHARACTER::Corin;
	m_pAnimator->Set_Animation(Get_Name() + "Idle")
		.Loop(true)
		.Apply();
	m_pCCT->Set_GravityEnabled(true);

	//m_pAnimator->Initialize_HumanoidRig();
	//CFootIK::FOOTIK_DESC ikDesc;
	//ikDesc.fRayStartOffset = 0.3f;
	//ikDesc.fRayDistance = 1.0f;
	//ikDesc.fMaxHeightDiff = 0.5f;
	//ikDesc.fMaxPelvisOffset = 0.1f;
	//ikDesc.iCollisionMask = 1 << ENUM(COLLISION_GROUP::COMMON);
	//ikDesc.bDynamicPoleVector = false;  // ²ô±â
	//ikDesc.vPoleVector = _vector3(0.f, 1.f, 0.f);  // °íÁ¤
	//m_pAnimator->Initialize_FootIK(&ikDesc);

	Initialize_Stat();
}

void CCorin::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CCorin::Update(_float dt)
{
	//Update_Input(dt);
	if(!m_bTest)
	{
		Update_States();
		m_pStateMachine->Update(dt);
	}
	__super::Update(dt);
}

void CCorin::Late_Update(_float dt)
{
	__super::Late_Update(dt);
	//Get_Component<CAnimator3D>()->Change_Speed(0.1f);
}

void CCorin::Render_GUI()
{
	__super::Render_GUI();
	if (m_pStateMachine)
	{
		ImGui::Separator();
		ImGui::Checkbox("Animation Test", &m_bTest);
		ImGui::Text("StateMachine: %s", m_pStateMachine->Get_CurrentStateName().c_str());

		if (ImGui::Button("Open StateMachine"))
			m_pStateMachine->Set_ShowWindow(true);

		m_pStateMachine->Render_GUI();

	}
	ImGui::Separator();
	ImGui::Text("Parrable Object %d", m_ParryableTargets.size());

}

void CCorin::On_SwitchIn(SWITCH eType)
{
	m_fDissolveProgress = 0.f;
	SetRenderLayer(RENDER_LAYER::Default);

	Set_Switch(eType);
	m_pStateMachine->Set_Trigger("SwitchIn");
}

void CCorin::On_SwitchOut()
{
	m_pStateMachine->Set_Trigger("SwitchOut");
}

void CCorin::On_Ultimate()
{
	__super::On_Ultimate();
	m_pStateMachine->Set_Int("AttackEntryMode", 3);
	m_pStateMachine->Set_Trigger("Attack");
}

void CCorin::On_Special()
{
	if (InputDevice()->Key_Tap('E') == false) return;

	if (m_tEnergy.fCurrentEnergy >= m_tEnergy.fSpecialEnergy)
	{
		UI_ACTION_DESC desc;
		desc.eType = UI_ACTION_TYPE::SPECIAL;
		desc.eState = UI_ACTION_STATE::EXECUTING;
		EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
	}

	string strCurrentState = m_pStateMachine->Get_CurrentStateName();
	if (strCurrentState == "Attack")	// NormalAttack Áß Äµ½½ÇØ¼­ ExAttack
	{
		CCorinState_Attack* pAttack = static_cast<CCorinState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (pAttack && pAttack->Get_SubStateMachine())
		{
			if (pAttack->Get_SubStateMachine()->Get_CurrentStateName() == "NormalAttack")
			{
				pAttack->Get_SubStateMachine()->Set_Trigger("ToExAttack");
				return;
			}
		}
	}
	m_pStateMachine->Set_Int("AttackEntryMode", 2);
	m_pStateMachine->Set_Trigger("Attack");
}

void CCorin::On_Hit(DAMAGE_TYPE eType)
{
	m_pStateMachine->Set_Int("HitEntryMode", ENUM(eType));
	m_pStateMachine->Set_Trigger("ToHit");
}

void CCorin::Update_States()
{
	m_pStateMachine->Set_Bool("IsMove", Is_Move_Buffer());
	
	Process_EndState(m_pStateMachine->Get_CurrentStateName());

	if (m_bIsEvade)
		m_pStateMachine->Set_Trigger("ToEvade");

	if (m_bIsAttack)
		Process_AttackInput(m_pStateMachine->Get_CurrentStateName());
}

void CCorin::Process_AttackInput(const string& strCurrentState)
{
	if (strCurrentState == "Idle")
	{	// Idle -> NormalAttack
		m_pStateMachine->Set_Int("AttackEntryMode", 0);
		m_pStateMachine->Set_Trigger("Attack");
	}
	else if (strCurrentState == "Move")
	{	// Move
		CCorinState_Move* pMove = static_cast<CCorinState_Move*>(
			m_pStateMachine->Get_CurrentState());
		if (!pMove || !pMove->Get_SubStateMachine())
			return;

		string strMoveType = pMove->Get_SubStateMachine()->Get_CurrentStateName();

		if (strMoveType == "Walk")	// Walk -> NormalAttack
			m_pStateMachine->Set_Int("AttackEntryMode", 0);
		else if (strMoveType == "Run")
		{
			IHState<CCorin>* pRun = dynamic_cast<IHState<CCorin>*>(
				pMove->Get_SubStateMachine()->Get_CurrentState());
			if (pRun && pRun->Get_SubStateMachine())
			{
				string strRunTag = pRun->Get_SubStateMachine()->Get_CurrentStateName();
				if (strRunTag == "End")
					m_pStateMachine->Set_Int("AttackEntryMode", 0);
				else
					m_pStateMachine->Set_Int("AttackEntryMode", 1);
			}
			else
				return;
		}
		else
			return;

		m_pStateMachine->Set_Trigger("Attack");
	}
	else if (strCurrentState == "Attack")
	{	// Attack
		CCorinState_Attack* pAttack = static_cast<CCorinState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (!pAttack || !pAttack->Get_SubStateMachine())
			return;

		if (pAttack->Get_SubStateMachine()->Get_CurrentStateName() != "NormalAttack")
			return;

		CCorinState_NormalAttack* pNormal = static_cast<CCorinState_NormalAttack*>(
			pAttack->Get_SubStateMachine()->Get_State("NormalAttack"));
		// NormalAttack : Combo
		if (pNormal && pNormal->Get_SubStateMachine())
			pNormal->Get_SubStateMachine()->Set_Trigger("NextCombo");
	}
}

void CCorin::Process_EndState(const string& strCurrentState)
{
	if (strCurrentState == "Move")
	{
		CCorinState_Move* pMove = static_cast<CCorinState_Move*>(
			m_pStateMachine->Get_CurrentState());
		if (!pMove) return;

		IHState<CCorin>* pMoveType = dynamic_cast<IHState<CCorin>*>(
			pMove->Get_SubStateMachine()->Get_CurrentState());
		if (pMoveType && pMoveType->Is_EndState())
		{
			IBaseState<CCorin>* pEnd = pMoveType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsAttack || m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	else if (strCurrentState == "Attack")
	{
		CCorinState_Attack* pAttack = static_cast<CCorinState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (!pAttack) return;

		IHState<CCorin>* pAttackType = dynamic_cast<IHState<CCorin>*>(
			pAttack->Get_SubStateMachine()->Get_CurrentState());
		if (pAttackType && pAttackType->Is_EndState())
		{
			IBaseState<CCorin>* pEnd = pAttackType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	else if (strCurrentState == "SwitchIn")
	{
		CCorinState_SwitchIn* pSwitchIn = static_cast<CCorinState_SwitchIn*>(
			m_pStateMachine->Get_CurrentState());
		if (!pSwitchIn) return;

		IHState<CCorin>* pSwitchInType = dynamic_cast<IHState<CCorin>*>(
			pSwitchIn->Get_SubStateMachine()->Get_CurrentState());
		if (pSwitchInType && pSwitchInType->Is_EndState())
		{
			IBaseState<CCorin>* pEnd = pSwitchInType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	else if (strCurrentState == "Hit")
	{
		CCorinState_Hit* pHit = static_cast<CCorinState_Hit*>(
			m_pStateMachine->Get_CurrentState());
		if (!pHit || !pHit->Get_SubStateMachine()) return;

		IBaseState<CCorin>* pHitType = pHit->Get_SubStateMachine()->Get_CurrentState();
		if (pHitType && pHitType->Get_AnimProgress() > 0.3f)
		{
			if (m_bIsEvade) return;
			if (Is_Input() || pHitType->Is_AnimEnd())
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
}


HRESULT CCorin::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CCorin>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CCorin::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CCorinState_Idle::Create());
	m_pStateMachine->Register_State("Move", CCorinState_Move::Create());
	m_pStateMachine->Register_State("Attack", CCorinState_Attack::Create());
	m_pStateMachine->Register_State("Evade", CCorinState_Evade::Create());
	m_pStateMachine->Register_State("SwitchIn", CCorinState_SwitchIn::Create());	//*SwitchIn*
	m_pStateMachine->Register_State("SwitchOut", CCorinState_SwitchOut::Create());//*SwtichOut*
	m_pStateMachine->Register_State("Hit", CCorinState_Hit::Create());

	return S_OK;
}

HRESULT CCorin::Initialize_Transitions()
{
	// Idle -> Move
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CCorin>::CONDITION_BOOL_TRUE, "IsMove");

	// Move -> Idle
	m_pStateMachine->Register_Transition("Move", "Idle",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

	// Attack
	m_pStateMachine->Register_AnyStateTransition("Attack",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "Attack");

	// Attack -> Idle
	m_pStateMachine->Register_Transition("Attack", "Idle",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

	// Evade
	m_pStateMachine->Register_AnyStateTransition("Evade",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToEvade");

	// Evade -> Move (Dash)
	m_pStateMachine->Register_Transition("Evade", "Move",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToMove");

	// Evade -> Idle (Backstep)
	m_pStateMachine->Register_Transition("Evade", "Idle",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

	// SwitchIn
	m_pStateMachine->Register_AnyStateTransition("SwitchIn",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "SwitchIn");

	m_pStateMachine->Register_Transition("SwitchIn", "Idle",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

	m_pStateMachine->Register_Transition("SwitchIn", "Move",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToMove");

	// SwitchOut
	m_pStateMachine->Register_AnyStateTransition("SwitchOut",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "SwitchOut");

	m_pStateMachine->Register_Transition("SwitchOut", "Idle",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

	// Hit
	m_pStateMachine->Register_AnyStateTransition("Hit",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToHit");

	m_pStateMachine->Register_Transition("Hit", "Idle",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

	return S_OK;
}

HRESULT CCorin::Initialize_Stat()
{
	auto Desc = CDataBase::GetInstance()->GetPlayerDesc(m_strName);
	m_tEnergy.fSpecialEnergy = Desc.SpecialAttack;

	auto LVDesc = CDataBase::GetInstance()->GetLevelDesc(m_iCurrentLevel);
	m_fMaxHP = LVDesc.MaxHP;
	m_fCurrentHP = m_fMaxHP;
	m_fDefense = LVDesc.Defend;
	m_fAttackPower = LVDesc.Attack;

	Set_EvadeMax(2);
	return S_OK;
}

HRESULT CCorin::Initialize_Weapon()
{
	ATTACK_COLLIDER_DESC desc;
	desc.eColliderType = COLLIDER_TYPE::BOX;
	desc.pOwnerAnimator = Get_Component<CAnimator3D>();
	desc.tagBone = "Weapon_saw";
	desc.tagName = "Saw";
	desc.vSize = { 0.5f, 0.5f, 0.05f };

	if (FAILED(Attach_AttackCollider(&desc)))	return E_FAIL;

	return S_OK;
}

CCorin* CCorin::Create()
{
	CCorin* pInstance = new CCorin();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CCorin::Clone(INIT_DESC* pArg)
{
	CCorin* pInstance = new CCorin(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CCorin::Free()
{
	Safe_Release(m_pStateMachine);
	__super::Free();
}
