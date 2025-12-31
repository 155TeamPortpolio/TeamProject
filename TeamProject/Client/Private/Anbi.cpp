#include "pch.h"
#include "Anbi.h"
#include "GameInstance.h"

#include "Material.h"

#include "Animator3D.h"
#include "CharacterController.h"

#include "StateMachine.h"
#include "AnbiState_Idle.h"
#include "AnbiState_Move.h"
#include "AnbiState_Attack.h"
#include "AnbiState_NormalAttack.h"

CAnbi::CAnbi()
{
}

CAnbi::CAnbi(const CAnbi& rhs)
	: CCharacter(rhs)
{
}

HRESULT CAnbi::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size02_Anbi.model",
		"../Bin/Resources/Model/skeletal/Anbi/Avatar_Female_Size02_Anbi.model");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size02_Anbi.mat",
		"../Bin/Resources/Model/skeletal/Anbi/Avatar_Female_Size02_Anbi.mat");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size02_Anbi.json",
		"../Bin/Resources/Model/skeletal/Anbi/Anim/Avatar_Female_Size02_Anbi_Meta.json");

	Get_Component<CModel>()->Link_Model("Test_Level", "Avatar_Female_Size02_Anbi.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "Avatar_Female_Size02_Anbi.mat");


	return S_OK;
}

HRESULT CAnbi::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CAnbi::Awake()
{
	m_pAnimator->LinkAnimate_Model("Test_Level", "Avatar_Female_Size02_Anbi.model");
	m_pAnimator->Link_MetaData("Test_Level", "Avatar_Female_Size02_Anbi.json");

	m_pAnimator->Set_MotionBone(12);
	m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	m_pAnimator->Set_Animation("Avatar_Female_Size02_Anbi_Ani_Idle")
		.Loop(true)
		.Apply();
	m_pCCT->Set_GravityEnabled(true);
}

void CAnbi::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CAnbi::Update(_float dt)
{
	Update_Input(dt);
	Update_States();
	m_pStateMachine->Update(dt);
	__super::Update(dt);
}

void CAnbi::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CAnbi::Render_GUI()
{
	__super::Render_GUI();
	if (m_pStateMachine)
	{
		ImGui::Separator();
		ImGui::Text("StateMachine: %s", m_pStateMachine->Get_CurrentStateName().c_str());

		if (ImGui::Button("Open StateMachine"))
			m_pStateMachine->Set_ShowWindow(true);

		m_pStateMachine->Render_GUI();
	}
}

void CAnbi::Update_Input(_float dt)
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

void CAnbi::Update_States()
{
	_bool bInMoveEnd = false;
	_bool bInAttackEnd = false;

	if (m_pStateMachine->Get_CurrentStateName() == "Move")
	{
		CAnbiState_Move* pMove =
			static_cast<CAnbiState_Move*>(m_pStateMachine->Get_CurrentState());

		if (pMove && pMove->Get_SubStateMachine())
		{
			IHState<CAnbi>* pMoveType =
				dynamic_cast<IHState<CAnbi>*>(pMove->Get_SubStateMachine()->Get_CurrentState());

			if (pMoveType && pMoveType->Has_SubStateMachine())
			{
				IBaseState<CAnbi>* pAnim =
					pMoveType->Get_SubStateMachine()->Get_CurrentState();

				bInMoveEnd = (pAnim && pAnim->Get_Tag() == "End");
			}
		}
	}
	// Attack End 체크
	else if (m_pStateMachine->Get_CurrentStateName() == "Attack")
	{
		CAnbiState_Attack* pAttack =
			static_cast<CAnbiState_Attack*>(m_pStateMachine->Get_CurrentState());

		if (pAttack && pAttack->Get_SubStateMachine())
		{
			string strSub = pAttack->Get_SubStateMachine()->Get_CurrentStateName();

			if (strSub == "NormalAttack")
			{
				CAnbiState_NormalAttack* pNormal =
					static_cast<CAnbiState_NormalAttack*>(
						pAttack->Get_SubStateMachine()->Get_State("NormalAttack"));

				if (pNormal && pNormal->Get_SubStateMachine())
				{
					IBaseState<CAnbi>* pNormalSub = pNormal->Get_SubStateMachine()->Get_CurrentState();
					bInAttackEnd = (pNormalSub && pNormalSub->Get_Tag() == "End");
				}
			}
		}
	}

	// AttackEnd 파라미터 설정
	if (bInAttackEnd)
	{
		CAnbiState_Attack* pAttack =
			static_cast<CAnbiState_Attack*>(m_pStateMachine->Get_CurrentState());

		if (pAttack)
		{
			// Attack의 AnimProgress가 1.0이면 AttackEnd = true
			_bool bAttackFinished = (pAttack->Get_AnimProgress() >= 1.f);
			m_pStateMachine->Set_Bool("AttackEnd", bAttackFinished);
		}
	}
	else
	{
		m_pStateMachine->Set_Bool("AttackEnd", false);
	}

	// End 캔슬 처리 (기존과 동일)
	if ((bInMoveEnd || bInAttackEnd) && m_bIsInput)
	{
		m_pStateMachine->Set_Bool("IsMove", false);

		// Attack End에서 입력 시 강제로 AttackEnd = true
		if (bInAttackEnd)
			m_pStateMachine->Set_Bool("AttackEnd", true);
	}
	else
	{
		m_pStateMachine->Set_Bool("IsMove", m_bIsMove);

		if (m_bIsAttack)
		{
			string strCurrent = m_pStateMachine->Get_CurrentStateName();

			if (strCurrent == "Idle")
			{
				m_pStateMachine->Set_Trigger("Attack");
			}
			else if (strCurrent == "Attack")
			{
				CAnbiState_Attack* pAttackState =
					static_cast<CAnbiState_Attack*>(m_pStateMachine->Get_CurrentState());
				if (pAttackState && pAttackState->Get_SubStateMachine())
				{
					if (pAttackState->Get_SubStateMachine()->Get_CurrentStateName() == "NormalAttack")
					{
						CAnbiState_NormalAttack* pNormal =
							static_cast<CAnbiState_NormalAttack*>(
								pAttackState->Get_SubStateMachine()->Get_State("NormalAttack"));

						if (pNormal && pNormal->Get_SubStateMachine())
							pNormal->Get_SubStateMachine()->Set_Trigger("NextCombo");
					}
				}
			}
		}
	}
}


HRESULT CAnbi::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CAnbi>::Create();
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

HRESULT CAnbi::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CAnbiState_Idle::Create());
	m_pStateMachine->Register_State("Move", CAnbiState_Move::Create());
	m_pStateMachine->Register_State("Attack", CAnbiState_Attack::Create());

	return S_OK;
}

HRESULT CAnbi::Initialize_Transitions()
{
	// Idle <-> Move
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CAnbi>::CONDITION_BOOL_TRUE, "IsMove");

	m_pStateMachine->Register_Transition("Move", "Idle",
		CStateMachine<CAnbi>::CONDITION_BOOL_FALSE, "IsMove");

	// Idle -> Attack
	m_pStateMachine->Register_AnyStateTransition("Attack",
		CStateMachine<CAnbi>::CONDITION_TRIGGER, "Attack");

	// Attack -> Idle
	m_pStateMachine->Register_Transition("Attack", "Idle",
		CStateMachine<CAnbi>::CONDITION_BOOL_TRUE, "AttackEnd");

	return S_OK;
}


CAnbi* CAnbi::Create()
{
	CAnbi* pInstance = new CAnbi();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CAnbi::Clone(INIT_DESC* pArg)
{
	CAnbi* pInstance = new CAnbi(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CAnbi::Free()
{
	Safe_Release(m_pStateMachine);
	__super::Free();
}