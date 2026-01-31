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

	pSubStateMachine->Set_Bool("ReserveNormal", false);

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
	for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
	{
		if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
		if (Event.Tag == "SawInterval")
		{
			pOwner->Begin_AttackCollider("Saw",
				HitDesc()
				.Type(HIT_TYPE::INTERVAL)
				.Damage(pOwner->Get_AttackPower() * 2.712 * Helper::Get_Random_Float(1.0, 1.5)
					, DAMAGE_TYPE::HARD)
				.Interval(0.05f)
				.Charge(5.f, 50.f)
			);
		}
	}

	// Explode 종료 시 NormalAttack 연계 체크
	if (m_pSubStateMachine->Get_CurrentStateName() == "Counter_Explode" &&
		m_pSubStateMachine->Get_Bool("ReserveNormal") &&
		Is_AnimEnd())
	{
		m_pSubStateMachine->Set_Bool("ReserveNormal", false);
		m_pParentState->Get_SubStateMachine()->Set_Trigger("ToNormalAttack");
		return;
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
	pOwner->Unlock_Move();
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
	pOwner->End_AttackCollider("Saw");
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