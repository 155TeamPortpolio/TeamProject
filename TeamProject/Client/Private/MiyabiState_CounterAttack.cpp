#include "pch.h"
#include "MiyabiState_CounterAttack.h"
#include "Miyabi.h"

CMiyabiState_CounterAttack* CMiyabiState_CounterAttack::Create()
{
	auto pInstance = new CMiyabiState_CounterAttack();
	pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
	auto pSubStateMachine = pInstance->Get_SubStateMachine();

	pSubStateMachine->Register_State("Counter_Start", CMiyabiState_Counter_Start::Create());
	pSubStateMachine->Register_State("Counter_End", CMiyabiState_Counter_End::Create());

	pSubStateMachine->Get_State("Counter_End")->Set_Tag("End");

	pSubStateMachine->Register_Transition("Counter_Start", "Counter_End",
		CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

	pSubStateMachine->Set_DefaultState("Counter_Start");

	return pInstance;
}

void CMiyabiState_CounterAttack::Enter(CMiyabi* pOwner)
{
	pOwner->Lock_Move();
	pOwner->Push_Invincible();
	__super::Enter(pOwner);
}

void CMiyabiState_CounterAttack::Update(CMiyabi* pOwner, _float dt)
{
	auto pMiyabiState = pOwner->Get_StateMachine();
	if (pMiyabiState->Get_Bool("OutReserve"))
	{
		if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" ||
			Is_AnimEnd())
		{
			pMiyabiState->Set_Trigger("SwitchOut");
			pMiyabiState->Set_Bool("OutReserve", false);
		}
	}

	__super::Update(pOwner, dt);
}

void CMiyabiState_CounterAttack::Exit(CMiyabi* pOwner)
{
	pOwner->Reset_ReserveCombo();
	pOwner->Pop_Invincible();
	pOwner->Unlock_Move();
	__super::Exit(pOwner);
}

void CMiyabiState_Counter_Start::Enter(CMiyabi* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter")
		.Speed(1.f)
		.Apply();
}

void CMiyabiState_Counter_Start::Update(CMiyabi* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
		ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Counter_End::Enter(CMiyabi* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter_End")
		.Speed(1.f)
		.Apply();
	pOwner->Unlock_Move();
}

void CMiyabiState_Counter_End::Update(CMiyabi* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
		ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}
