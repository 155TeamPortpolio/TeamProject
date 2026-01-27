#include "pch.h"
#include "CorinState_CounterAttack.h"
#include "Corin.h"

CCorinState_CounterAttack* CCorinState_CounterAttack::Create()
{
	auto pInstance = new CCorinState_CounterAttack();
	pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
	auto pSubStateMachine = pInstance->Get_SubStateMachine();

	pSubStateMachine->Register_State("Counter_Start", CCorinState_Counter_Start::Create());
	pSubStateMachine->Register_State("Counter_Explode", CCorinState_Counter_Explode::Create());
	pSubStateMachine->Register_State("Counter_End", CCorinState_Counter_End::Create());

	pSubStateMachine->Get_State("Counter_End")->Set_Tag("End");

	pSubStateMachine->Register_Transition("Counter_Start", "Counter_Explode",
		CStateMachine<CCorin>::CONDITION_ANIMATION_END);
	pSubStateMachine->Register_Transition("Counter_Explode", "Counter_End",
		CStateMachine<CCorin>::CONDITION_ANIMATION_END);

	pSubStateMachine->Set_DefaultState("Counter_Start");

	return pInstance;
}

void CCorinState_CounterAttack::Enter(CCorin* pOwner)
{
	pOwner->Lock_Move();
	pOwner->Push_Invincible();
	__super::Enter(pOwner);
}

void CCorinState_CounterAttack::Update(CCorin* pOwner, _float dt)
{
	// TODO : 클립 이벤트 추가
	for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
	{
		if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

		if (Event.Tag == "LHandStart")
		{
			pOwner->Begin_AttackCollider("HandWeapon_L", { HIT_TYPE::ONCE, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(10,30), 0, 0 });
		}
		else if (Event.Tag == "LHandEnd")
		{
			pOwner->End_AttackCollider("HandWeapon_L");
		}
	}

	auto pCorinState = pOwner->Get_StateMachine();
	if (pCorinState->Get_Bool("OutReserve"))
	{
		if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" ||
			Is_AnimEnd())
		{
			pCorinState->Set_Trigger("SwitchOut");
			pCorinState->Set_Bool("OutReserve", false);
		}
	}

	__super::Update(pOwner, dt);
}

void CCorinState_CounterAttack::Exit(CCorin* pOwner)
{
	pOwner->Pop_Invincible();
	__super::Exit(pOwner);
}

void CCorinState_Counter_Start::Enter(CCorin* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter")
		.Speed(1.5f)
		.Apply();
}

void CCorinState_Counter_Start::Update(CCorin* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_Counter_Explode::Enter(CCorin* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter_Explode")
		.Speed(1.5f)
		.Apply();
}

void CCorinState_Counter_Explode::Update(CCorin* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_Counter_End::Enter(CCorin* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter_End")
		.Speed(1.2f)
		.Apply();
	pOwner->Unlock_Move();
}

void CCorinState_Counter_End::Update(CCorin* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}