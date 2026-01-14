#include "pch.h"
#include "JaneDoe.h"
#include "GameInstance.h"
#include "DataBase.h"

#include "Material.h"
#include "MaterialInstance.h"

#include "Animator3D.h"
#include "CharacterController.h"

#include "StateMachine.h"
#include "JaneDoeState_Idle.h"
#include "JaneDoeState_Move.h"
#include "JaneDoeState_Attack.h"
#include "JaneDoeState_SwitchIn.h"
#include "JaneDoeState_SwitchOut.h"
#include "JaneDoeState_NormalAttack.h"
#include "JaneDoeState_Evade.h"

CJaneDoe::CJaneDoe()
{
}

CJaneDoe::CJaneDoe(const CJaneDoe& rhs)
	:CCharacter(rhs)
{
}

void CJaneDoe::ProcessPassion(_float fPassionGauge)
{
	m_fPassionGauge = fPassionGauge;
}

void CJaneDoe::ProcessPassionSkill(_bool bAvailable)
{
	m_bPassionSkillAvailable = bAvailable;
}

HRESULT CJaneDoe::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("JaneDoe.model",
		"../Bin/Resources/Model/skeletal/JaneDoe/JaneDoe.model");
	pRcsMgr->Add_ResourcePath("JaneDoe.mat",
		"../Bin/Resources/Model/skeletal/JaneDoe/JaneDoe.mat");
	pRcsMgr->Add_ResourcePath("JaneDoe_Meta.json",
		"../Bin/Resources/Model/skeletal/JaneDoe/JaneDoe_Meta.json");

	Get_Component<CModel>()->Link_Model("Test_Level", "JaneDoe.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "JaneDoe.mat");

	return S_OK;
}

HRESULT CJaneDoe::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;



	return S_OK;
}

void CJaneDoe::Awake()
{
	__super::Awake();

	m_pAnimator->LinkAnimate_Model("Test_Level", "JaneDoe.model");
	m_pAnimator->Link_MetaData("Test_Level", "JaneDoe_Meta.json");

	//m_pAnimator->Set_MotionBone(262);
	m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);

	m_strAnimName = "Avatar_Female_Size03_JaneDoe_Ani_";
	m_strName = "JaneDoe";
	m_pAnimator->Set_Animation(Get_Name() + "Idle")
		.Loop(true)
		.Apply();
	m_pCCT->Set_GravityEnabled(true);

	Initialize_Stat();

}

void CJaneDoe::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CJaneDoe::Update(_float dt)
{
	//Update_Input(dt);
	if (!m_bTest)
	{
		Update_States();
		m_pStateMachine->Update(dt);
	}
	__super::Update(dt);
}

void CJaneDoe::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CJaneDoe::Render_GUI()
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
}

void CJaneDoe::On_SwitchIn(SWITCH eType)
{
	m_fDissolveProgress = 0.f;
	SetRenderLayer(RENDER_LAYER::Default);

	Set_Switch(eType);
	m_pStateMachine->Set_Trigger("SwitchIn");
}

void CJaneDoe::On_SwitchOut()
{
	m_pStateMachine->Set_Trigger("SwitchOut");
}

HRESULT CJaneDoe::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CJaneDoe>::Create();
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

HRESULT CJaneDoe::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CJaneDoeState_Idle::Create());
	m_pStateMachine->Register_State("Move", CJaneDoeState_Move::Create());
	m_pStateMachine->Register_State("Attack", CJaneDoeState_Attack::Create());
	m_pStateMachine->Register_State("Evade", CJaneDoeState_Evade::Create());
	m_pStateMachine->Register_State("SwitchIn", CJaneDoeState_SwitchIn::Create());	//*SwitchIn*
	m_pStateMachine->Register_State("SwitchOut", CJaneDoeState_SwitchOut::Create());//*SwtichOut*

	return S_OK;
}

HRESULT CJaneDoe::Initialize_Transitions()
{
	// Idle -> Move
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CJaneDoe>::CONDITION_BOOL_TRUE, "IsMove");

	// Move -> Idle
	m_pStateMachine->Register_Transition("Move", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	// Attack
	m_pStateMachine->Register_AnyStateTransition("Attack",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "Attack");

	// Attack -> Idle
	m_pStateMachine->Register_Transition("Attack", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	// Evade
	m_pStateMachine->Register_AnyStateTransition("Evade",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToEvade");

	// Evade -> Move (Dash)
	m_pStateMachine->Register_Transition("Evade", "Move",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToMove");

	// Evade -> Idle (Backstep)
	m_pStateMachine->Register_Transition("Evade", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	// SwitchIn
	m_pStateMachine->Register_AnyStateTransition("SwitchIn",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "SwitchIn");

	m_pStateMachine->Register_Transition("SwitchIn", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	// SwitchOut
	m_pStateMachine->Register_AnyStateTransition("SwitchOut",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "SwitchOut");

	m_pStateMachine->Register_Transition("SwitchOut", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");
	return S_OK;
}

HRESULT CJaneDoe::Initialize_Stat()
{
	auto Desc = CDataBase::GetInstance()->GetPlayerDesc(m_strName);
	m_tEnergy.fCurrentEnergy = Desc.SpecialAttack;

	auto LVDesc = CDataBase::GetInstance()->GetLevelDesc(m_iCurrentLevel);
	m_fMaxHP = LVDesc.MaxHP;
	m_fDefense = LVDesc.Defend;
	m_fAttackPower = LVDesc.Attack;

	m_iEvadeMax = 3;

	return S_OK;
}

void CJaneDoe::Update_States()
{
	m_pStateMachine->Set_Bool("IsMove", Is_Move_Buffer());

	Process_EndState(m_pStateMachine->Get_CurrentStateName());

	if (m_bIsEvade)
		m_pStateMachine->Set_Trigger("ToEvade");

	if (m_bIsAttack)
		Process_AttackInput(m_pStateMachine->Get_CurrentStateName());
}

void CJaneDoe::Process_AttackInput(const string& strCurrentState)
{
	if (strCurrentState == "Idle")
	{	// Idle -> NormalAttack
		m_pStateMachine->Set_Int("AttackEntryMode", 0);
		m_pStateMachine->Set_Trigger("Attack");
	}
	else if (strCurrentState == "Move")
	{	// Move
		CJaneDoeState_Move* pMove = static_cast<CJaneDoeState_Move*>(
			m_pStateMachine->Get_CurrentState());
		if (!pMove || !pMove->Get_SubStateMachine())
			return;

		string strMoveType = pMove->Get_SubStateMachine()->Get_CurrentStateName();

		if (strMoveType == "Walk")	// Walk -> NormalAttack
			m_pStateMachine->Set_Int("AttackEntryMode", 0);
		else if (strMoveType == "Run")
		{
			IHState<CJaneDoe>* pRun = dynamic_cast<IHState<CJaneDoe>*>(
				pMove->Get_SubStateMachine()->Get_CurrentState());
			if (pRun && pRun->Get_SubStateMachine())
			{
				string strRunTag = pRun->Get_SubStateMachine()->Get_CurrentStateName();
				if (strRunTag == "End") m_pStateMachine->Set_Int("AttackEntryMode", 0);
				else m_pStateMachine->Set_Int("AttackEntryMode", 1);
			}
			else return;
		}
		else return;

		m_pStateMachine->Set_Trigger("Attack");
	}
	else if (strCurrentState == "Attack")
	{	// Attack
		CJaneDoeState_Attack* pAttack = static_cast<CJaneDoeState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (!pAttack || !pAttack->Get_SubStateMachine())
			return;

		if (pAttack->Get_SubStateMachine()->Get_CurrentStateName() != "NormalAttack")
			return;

		CJaneDoeState_NormalAttack* pNormal = static_cast<CJaneDoeState_NormalAttack*>(
			pAttack->Get_SubStateMachine()->Get_State("NormalAttack"));
		// NormalAttack : Combo
		if (pNormal && pNormal->Get_SubStateMachine())
			pNormal->Get_SubStateMachine()->Set_Trigger("NextCombo");
	}
}

void CJaneDoe::Process_EndState(const string& strCurrentState)
{
	if (strCurrentState == "Move")
	{
		CJaneDoeState_Move* pMove = static_cast<CJaneDoeState_Move*>(
			m_pStateMachine->Get_CurrentState());
		if (!pMove) return;

		IHState<CJaneDoe>* pMoveType = dynamic_cast<IHState<CJaneDoe>*>(
			pMove->Get_SubStateMachine()->Get_CurrentState());
		if (pMoveType && pMoveType->Is_EndState())
		{
			IBaseState<CJaneDoe>* pEnd = pMoveType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsAttack || m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	else if (strCurrentState == "Attack")
	{
		CJaneDoeState_Attack* pAttack = static_cast<CJaneDoeState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (!pAttack) return;

		IHState<CJaneDoe>* pAttackType = dynamic_cast<IHState<CJaneDoe>*>(
			pAttack->Get_SubStateMachine()->Get_CurrentState());
		if (pAttackType && pAttackType->Is_EndState())
		{
			IBaseState<CJaneDoe>* pEnd = pAttackType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
}

CJaneDoe* CJaneDoe::Create()
{
	CJaneDoe* Instance = new CJaneDoe();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CJaneDoe::Clone(INIT_DESC* pArg)
{
	CJaneDoe* Instance = new CJaneDoe(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CJaneDoe::Free()
{
	Safe_Release(m_pStateMachine);
	__super::Free();
}
