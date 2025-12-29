#include "pch.h"
#include "SacrificeState_Walk.h"
#include "Sacrifice.h"

/* Sub States */
#include "SacrificeState_Walk_Phase1.h"
#include "CharacterController.h"

void CSacrificeState_Walk::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		m_pSubStateMachine->Register_State("Phase1", CSacrificeState_Walk_Phase1::Create());
		m_pSubStateMachine->Set_DefaultState("Phase1");
	}
	__super::Enter(pOwner);
}

void CSacrificeState_Walk::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	auto pCCT = pOwner->Get_Component<CCharacterController>();
	_vector3 vRight = pOwner->Get_Component<CTransform>()->Dir(STATE::RIGHT);
	_vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
	_vector3 vDeltaMove = pAnimator->Get_RootBoneMoveDelta();
	vDeltaMove.y = 0.f;
	vDeltaMove = vRight * vDeltaMove.x + vLook * -1.f * vDeltaMove.z;

	pCCT->Move_RootMotion(vDeltaMove * 0.5f, _vector4(0.f, 0.f, 0.f, 1.f), dt);

	if (m_fStateTime >= 1.5f)
		pOwner->Idle();
}

void CSacrificeState_Walk::Exit(CSacrifice* pOwner)
{
}
