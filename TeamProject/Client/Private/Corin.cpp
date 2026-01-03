#include "pch.h"
#include "Corin.h"
#include "GameInstance.h"

#include "Material.h"

#include "Animator3D.h"
#include "CharacterController.h"

#include "StateMachine.h"
#include "CorinState_Idle.h"
#include "CorinState_Move.h"
#include "CorinState_Attack.h"
#include "CorinState_NormalAttack.h"
#include "CorinState_Evade.h"

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
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size01_Corin.model",
		"../Bin/Resources/Model/skeletal/Corin/Avatar_Female_Size01_Corin.model");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size01_Corin.mat",
		"../Bin/Resources/Model/skeletal/Corin/Avatar_Female_Size01_Corin.mat");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size01_Corin.json",
		"../Bin/Resources/Model/skeletal/Corin/Avatar_Female_Size01_Corin_Meta.json");

	Get_Component<CModel>()->Link_Model("Test_Level", "Avatar_Female_Size01_Corin.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "Avatar_Female_Size01_Corin.mat");

	return S_OK;
}

HRESULT CCorin::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CCorin::Awake()
{
	m_pAnimator->LinkAnimate_Model("Test_Level", "Avatar_Female_Size01_Corin.model");
	m_pAnimator->Link_MetaData("Test_Level", "Avatar_Female_Size01_Corin.json");

	m_pAnimator->Set_MotionBone(12);
	m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	m_pAnimator->Set_Animation("Avatar_Female_Size01_Corin_Ani_Idle")
		.Loop(true)
		.Apply();
	m_pCCT->Set_GravityEnabled(true);
}

void CCorin::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CCorin::Update(_float dt)
{
	Update_Input(dt);
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
}

void CCorin::Process_RootMotion(_float dt, const ROOTMOTION_DESC& desc)
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

void CCorin::Process_RootMotion(_float dt, _uint iModeMask)
{
	ROOTMOTION_DESC desc;
	desc.iModeMask = iModeMask;
	Process_RootMotion(dt, desc);
}

void CCorin::Update_Input(_float dt)
{
	__super::Update_Input(dt);

	auto input = CGameInstance::GetInstance()->Get_InputDev();
	// 디버그용 레이캐스트
	//if (input->Key_Hold('F'))
	//{
	//	PHYSICS_RAY_HIT hit;
	//	_vector vLook = m_pTransform->Dir(STATE::LOOK);
	//	m_pCCT->Shoot_Ray(vLook, 100.f, hit);
	//}
	//else
	//{
	//	m_pCCT->Clear_DebugRay();
	//}

	// 테스트용 점프 (J키)
	if (input->Key_Down('J'))
	{
		m_pCCT->Jump(3.f);
	}
}

void CCorin::Update_States()
{
	if (m_bIsEvade)	// Evade 입력 업데이트
		m_pStateMachine->Set_Trigger("ToEvade");

	string strCurrent = m_pStateMachine->Get_CurrentStateName();

	_bool bInMoveEnd = false;
	_bool bInAttackEnd = false;
	// End상태 체크
	if (strCurrent == "Move")
	{	// Move의 End상태 체크
		CCorinState_Move* pMove = static_cast<CCorinState_Move*>(
			m_pStateMachine->Get_CurrentState());
		if (pMove)
			bInMoveEnd = pMove->Is_EndState();
	}
	else if (strCurrent == "Attack")
	{	// Attack의 End상태 체크
		CCorinState_Attack* pAttack = static_cast<CCorinState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (pAttack)
			bInAttackEnd = pAttack->Is_EndState();
	}

	if ((bInMoveEnd || bInAttackEnd) && m_bIsInput)
	{	// End상태일때 애니매이션 캔슬
		m_pStateMachine->Set_Bool("IsMove", false);
		if (bInAttackEnd)
			m_pStateMachine->Set_Bool("AttackEnd", true);
	}
	else
	{	// End상태가 아니라면 : 일반 업데이트
		m_pStateMachine->Set_Bool("IsMove", m_bIsMove);
		// 공격 입력 처리
		if (m_bIsAttack) Process_AttackInput(strCurrent);
	}
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
		else if (strMoveType == "Run")	// Run -> RushAttack
			m_pStateMachine->Set_Int("AttackEntryMode", 1);
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

	return S_OK;
}

HRESULT CCorin::Initialize_Transitions()
{
	// Idle <-> Move
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CCorin>::CONDITION_BOOL_TRUE, "IsMove");
	m_pStateMachine->Register_Transition("Move", "Idle",
		CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");

	// Attack
	m_pStateMachine->Register_AnyStateTransition("Attack",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "Attack");

	// Attack -> Idle
	m_pStateMachine->Register_Transition("Attack", "Idle",
		CStateMachine<CCorin>::CONDITION_BOOL_TRUE, "AttackEnd");

	// Evade
	m_pStateMachine->Register_AnyStateTransition("Evade",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToEvade");

	// Evade → Move (Dash 종료)
	m_pStateMachine->Register_Transition("Evade", "Move",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToMove");

	// Evade → Idle (Backstep 종료)
	m_pStateMachine->Register_Transition("Evade", "Idle",
		CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

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
