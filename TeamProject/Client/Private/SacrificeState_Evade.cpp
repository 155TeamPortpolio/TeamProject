#include "pch.h"
#include "SacrificeState_Evade.h"
#include "Sacrifice.h"

/* Component */
#include "CharacterController.h"

/* Sub States */
#include "SacrificeState_Evade_Phase1.h"

void CSacrificeState_Evade::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Evade_Phase1::Create());
		m_pSubStateMachine->Set_DefaultState("Phase1");
	}

	__super::Enter(pOwner);
}

void CSacrificeState_Evade::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		pOwner->Idle();
	}
	else
	{
		auto pCCT = pOwner->Get_Component<CCharacterController>();
		_vector3 vRight = pOwner->Get_Component<CTransform>()->Dir(STATE::RIGHT);
		_vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
		_vector3 vDeltaMove = pAnimator->Get_RootBoneMoveDelta();
		vDeltaMove.y = 0.f;
		vDeltaMove = vRight * vDeltaMove.x + vLook * -1.f * vDeltaMove.z;

		pCCT->Move_RootMotion(vDeltaMove, _vector4(0.f, 0.f, 0.f, 1.f), dt);
	}
}

void CSacrificeState_Evade::Exit(CSacrifice* pOwner)
{
}
