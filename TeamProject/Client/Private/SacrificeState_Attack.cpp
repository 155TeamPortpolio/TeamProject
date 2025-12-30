#include "pch.h"
#include "SacrificeState_Attack.h"
#include "Sacrifice.h"
#include "Helper_Func.h"
#include "CharacterController.h"

/* State */
#include "SacrificeState_Attack_Phase1.h"
#include "SacrificeState_Attack_Phase2.h"

void CSacrificeState_Attack::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		/* States */
		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Attack_Phase1::Create());
		m_pSubStateMachine->Register_State("Phase2", CSacrificeState_Attack_Phase2::Create());
		
		/* Transitions */
		m_pSubStateMachine->Register_Transition("Phase1", "Phase2",
			CStateMachine<CSacrifice>::CONDITION_TRIGGER, "ChangePhase");

		m_pSubStateMachine->Set_DefaultState("Phase1");
	}

	__super::Enter(pOwner);
}

void CSacrificeState_Attack::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	auto pCCT = pOwner->Get_Component<CCharacterController>();
	_vector3 vRight = pOwner->Get_Component<CTransform>()->Dir(STATE::RIGHT);
	_vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
	_vector3 vDeltaMove = pAnimator->Get_RootBoneMoveDelta();
	vDeltaMove.y = 0.f;
	vDeltaMove = (vRight * vDeltaMove.x + vLook * vDeltaMove.z) * -1.f;

	pCCT->Move_RootMotion(vDeltaMove, _vector4(0.f, 0.f, 0.f, 1.f), dt);
}

void CSacrificeState_Attack::Exit(CSacrifice* pOwner)
{
}