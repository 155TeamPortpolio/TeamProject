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

void CCorin::Update_Input(_float dt)
{
	__super::Update_Input(dt);

	// 디버그용 레이캐스트 (F키)
	auto input = CGameInstance::GetInstance()->Get_InputDev();
	if (input->Key_Hold('F'))
	{
		PHYSICS_RAY_HIT hit;
		_vector vLook = m_pTransform->Dir(STATE::LOOK);
		m_pCCT->Shoot_Ray(vLook, 100.f, hit);
	}
	else
	{
		m_pCCT->Clear_DebugRay();
	}

	// 테스트용 점프 (J키)
	if (input->Key_Down('J'))
	{
		m_pCCT->Jump(3.f);
	}
}

void CCorin::Update_States()
{
	_bool bInMoveEnd = false;
	_bool bInAttackEnd = false;

	if (m_pStateMachine->Get_CurrentStateName() == "Move")
	{	// Move의 End 애니매이션 중인지 확인
		CCorinState_Move* pMove = static_cast<CCorinState_Move*>(m_pStateMachine->Get_CurrentState());
		if (pMove && pMove->Get_SubStateMachine())
		{
			IHState<CCorin>* pMoveType = dynamic_cast<IHState<CCorin>*>(pMove->Get_SubStateMachine()->Get_CurrentState());
			if (pMoveType && pMoveType->Has_SubStateMachine())
			{
				IBaseState<CCorin>* pAnim = pMoveType->Get_SubStateMachine()->Get_CurrentState();
				bInMoveEnd = (pAnim && pAnim->Get_Tag() == "End");
			}
		}
	}
	else if (m_pStateMachine->Get_CurrentStateName() == "Attack")
	{	// Attack의 End 애니매이션 중인지 확인
		CCorinState_Attack* pAttack = static_cast<CCorinState_Attack*>(m_pStateMachine->Get_CurrentState());
		if (pAttack && pAttack->Get_SubStateMachine())
		{
			IHState<CCorin>* pAttackType = dynamic_cast<IHState<CCorin>*>(pAttack->Get_SubStateMachine()->Get_CurrentState());
			if (pAttackType && pAttackType->Has_SubStateMachine())
			{
				IBaseState<CCorin>* pAnim = pAttackType->Get_SubStateMachine()->Get_CurrentState();
				bInAttackEnd = (pAnim && pAnim->Get_Tag() == "End");
			}
		}
	}

	// AttackEnd 파라미터 업데이트
	// End 애니매이션이 끝까지 재생되었으면 AttackEnd = true
	if (bInAttackEnd)
	{
		CCorinState_Attack* pAttack = static_cast<CCorinState_Attack*>(m_pStateMachine->Get_CurrentState());
		if (pAttack)
		{
			_bool bAttackFinished = (pAttack->Get_AnimProgress() >= 1.f);
			m_pStateMachine->Set_Bool("AttackEnd", bAttackFinished);
		}
	}
	else
	{
		m_pStateMachine->Set_Bool("AttackEnd", false);
	}

	// Evade 트리거 업데이트
	if (m_bIsEvade)
		m_pStateMachine->Set_Trigger("ToEvade");


	if ((bInMoveEnd || bInAttackEnd) && m_bIsInput)
	{	// End 재생중일때 인풋이 들어오면 캔슬
		m_pStateMachine->Set_Bool("IsMove", false);
		if (bInAttackEnd) m_pStateMachine->Set_Bool("AttackEnd", true);
	}
	else
	{
		m_pStateMachine->Set_Bool("IsMove", m_bIsMove);
		if (m_bIsAttack)	// 공격입력했을때
		{
			string strCurrent = m_pStateMachine->Get_CurrentStateName();
			if (strCurrent == "Idle")	
			{	// Idle에서는 NormalAttack로 전환
				m_pStateMachine->Set_Int("AttackEntryMode", 0);
				m_pStateMachine->Set_Trigger("Attack");
			}
			else if (strCurrent == "Move")
			{	// Move일때는 Move의 서브 상태를 가져와서 Walk인경우 EntryMode 0, Run인경우 1
				// AttackEntryMode 0 : 노말어택, AttackEntryMode 1 : 러쉬어택
				CCorinState_Move* pMoveState = static_cast<CCorinState_Move*>(m_pStateMachine->Get_CurrentState());
				if (pMoveState && pMoveState->Get_SubStateMachine())
				{
					if (pMoveState->Get_SubStateMachine()->Get_CurrentStateName() == "Walk")
					{
						m_pStateMachine->Set_Int("AttackEntryMode", 0);
						m_pStateMachine->Set_Trigger("Attack");
					}
					else if (pMoveState->Get_SubStateMachine()->Get_CurrentStateName() == "Run")
					{
						m_pStateMachine->Set_Int("AttackEntryMode", 1);
						m_pStateMachine->Set_Trigger("Attack");
					}
				}
			}
			else if (strCurrent == "Attack")
			{	// NormalAttack중일때는 콤보어택 
				CCorinState_Attack* pAttackState = static_cast<CCorinState_Attack*>(m_pStateMachine->Get_CurrentState());
				if (pAttackState && pAttackState->Get_SubStateMachine())
				{
					if (pAttackState->Get_SubStateMachine()->Get_CurrentStateName() == "NormalAttack")
					{
						CCorinState_NormalAttack* pNormal = static_cast<CCorinState_NormalAttack*>(
								pAttackState->Get_SubStateMachine()->Get_State("NormalAttack")
							);
						if (pNormal && pNormal->Get_SubStateMachine())
							pNormal->Get_SubStateMachine()->Set_Trigger("NextCombo");
					}
				}
			}
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
