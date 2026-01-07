#include "pch.h"
#include "JaneDoe.h"
#include "GameInstance.h"

#include "Material.h"

#include "Animator3D.h"
#include "CharacterController.h"

#include "StateMachine.h"
#include "JaneDoeState_Idle.h"
#include "JaneDoeState_Move.h"
#include "JaneDoeState_Attack.h"
#include "JaneDoeState_NormalAttack.h"
#include "JaneDoeState_Evade.h"

CJaneDoe::CJaneDoe()
{
}

CJaneDoe::CJaneDoe(const CJaneDoe& rhs)
	:CCharacter(rhs)
{
}

HRESULT CJaneDoe::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size03_JaneDoe.model",
		"../Bin/Resources/Model/skeletal/ZaneDoe/Avatar_Female_Size03_JaneDoe.model");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size03_JaneDoe.mat",
		"../Bin/Resources/Model/skeletal/ZaneDoe/Avatar_Female_Size03_JaneDoe.mat");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size03_JaneDoe.json",
		"../Bin/Resources/Model/skeletal/ZaneDoe/Avatar_Female_Size03_JaneDoe_Meta.json");

	Get_Component<CModel>()->Link_Model("Test_Level", "Avatar_Female_Size03_JaneDoe.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "Avatar_Female_Size03_JaneDoe.mat");

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
	m_pAnimator->LinkAnimate_Model("Test_Level", "Avatar_Female_Size03_JaneDoe.model");
	m_pAnimator->Link_MetaData("Test_Level", "Avatar_Female_Size03_JaneDoe.json");

	m_pAnimator->Set_MotionBone(16);
	m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);

	m_strName = "Avatar_Female_Size03_JaneDoe_Ani_";
	m_pAnimator->Set_Animation(Get_Name() + "Idle")
		.Loop(true)
		.Apply();
	m_pCCT->Set_GravityEnabled(true);
}

void CJaneDoe::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CJaneDoe::Update(_float dt)
{
	Update_Input(dt);
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

void CJaneDoe::Process_RootMotion(_float dt, const ROOTMOTION_DESC& desc)
{
	auto pTransform = Get_Component<CTransform>();
	_vector3 vRootDelta = m_pAnimator->Get_RootBoneMoveDelta();
	_vector4 vQuatDelta = m_pAnimator->Get_RootBoneQuatDelta();
	_vector3 vInputDir = Get_InputDir();

	if ((desc.iModeMask & ENUM(ROOTMOTION_MASK::QUATERNION)) != 0)
	{
		if (desc.fRotateWeight >= 0.99f) pTransform->Add_Quaternion(vQuatDelta);
		else if (desc.fRotateWeight > 0.01f)
		{
			_quaternion qWeighted = _quaternion::Slerp(_quaternion::Identity, vQuatDelta, desc.fRotateWeight);
			pTransform->Add_Quaternion(qWeighted);
		}
	}
	else
	{
		if (vInputDir.Length() > 0.01f)
		{
			vInputDir.Normalize();
			Rotate(vInputDir);
		}
	}

	if ((desc.iModeMask & ENUM(ROOTMOTION_MASK::MOVE)) != 0)
	{
		if (vRootDelta.x != 0.f || vRootDelta.z != 0.f)
		{
			_vector3 vWeightedDelta = vRootDelta * desc.fMoveWeight;
			_quaternion qRot = pTransform->Get_QuaternionRotate();
			m_pCCT->Move_RootMotion(vWeightedDelta, qRot, dt);
		}
	}
	else
	{
		if (vInputDir.Length() > 0.01f)
		{
			vInputDir.Normalize();
			m_pCCT->Move_Direction(vInputDir, desc.fMoveSpeed, dt);
		}
	}
}

void CJaneDoe::Process_RootMotion(_float dt, _uint iModeMask)
{
	ROOTMOTION_DESC desc;
	desc.iModeMask = iModeMask;
	Process_RootMotion(dt, desc);
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

	return S_OK;
}

void CJaneDoe::Update_Input(_float dt)
{
	__super::Update_Input(dt);

	auto input = CGameInstance::GetInstance()->Get_InputDev();
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
			if (pEnd && (m_bIsInput || pEnd->Is_AnimEnd()))
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
			if (pEnd && (m_bIsInput || pEnd->Is_AnimEnd()))
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
