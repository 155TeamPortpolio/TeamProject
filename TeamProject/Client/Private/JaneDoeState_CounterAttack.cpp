#include "pch.h"
#include "JaneDoeState_CounterAttack.h"
#include "JaneDoe.h"

CJaneDoeState_CounterAttack* CJaneDoeState_CounterAttack::Create()
{
	auto pInstance = new CJaneDoeState_CounterAttack();
	pInstance->m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
	auto pSubStateMachine = pInstance->Get_SubStateMachine();

	pSubStateMachine->Register_State("Counter_01", CJaneDoeState_Counter_01::Create());
	pSubStateMachine->Register_State("Counter_02", CJaneDoeState_Counter_02::Create());
	pSubStateMachine->Register_State("Counter_03", CJaneDoeState_Counter_03::Create());
	pSubStateMachine->Register_State("Counter_End", CJaneDoeState_Counter_End::Create());

	pSubStateMachine->Get_State("Counter_End")->Set_Tag("End");

	pSubStateMachine->Register_Transition("Counter_01", "Counter_End",
		CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
	pSubStateMachine->Register_Transition("Counter_02", "Counter_End",
		CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
	pSubStateMachine->Register_Transition("Counter_03", "Counter_End",
		CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

	pSubStateMachine->Set_DefaultState("Counter_01");

	return pInstance;
}

void CJaneDoeState_CounterAttack::Enter(CJaneDoe* pOwner)
{
	pOwner->Lock_Move();
	pOwner->Push_Invincible();
	if (pOwner->Is_Passion())
	{
		m_pSubStateMachine->Set_DefaultState("Counter_03");
		m_pSubStateMachine->Set_Int("CounterMode", 0);
	}
	else if (pOwner->Get_EvadeCount() <= 1)
	{
		m_pSubStateMachine->Set_DefaultState("Counter_01");
		m_pSubStateMachine->Set_Int("CounterMode", 2);
	}
	else
	{
		m_pSubStateMachine->Set_DefaultState("Counter_02");
		m_pSubStateMachine->Set_Int("CounterMode", 1);
	}

	__super::Enter(pOwner);
}

void CJaneDoeState_CounterAttack::Update(CJaneDoe* pOwner, _float dt)
{
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
		else if (Event.Tag == "RHandStart")
		{
			pOwner->Begin_AttackCollider("HandWeapon_R", { HIT_TYPE::ONCE, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(10,30), 0, 0 });
		}
		else if (Event.Tag == "RHandEnd")
		{
			pOwner->End_AttackCollider("HandWeapon_R");
		}
		else if (Event.Tag == "LFootStart")
		{
			pOwner->Begin_AttackCollider("FootWeapon_L", { HIT_TYPE::ONCE, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(20,40), 0, 0 });
		}
		else if (Event.Tag == "LFootEnd")
		{
			pOwner->End_AttackCollider("FootWeapon_L");
		}
		else if (Event.Tag == "RFootStart")
		{
			pOwner->Begin_AttackCollider("FootWeapon_R", { HIT_TYPE::ONCE, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(20,40), 0, 0 });
		}
		else if (Event.Tag == "RFootEnd")
		{
			pOwner->End_AttackCollider("FootWeapon_R");
		}
	}

	auto pJaneDoeState = pOwner->Get_StateMachine();
	if (pJaneDoeState->Get_Bool("OutReserve"))
	{
		if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" ||
			Is_AnimEnd())
		{
			pJaneDoeState->Set_Trigger("SwitchOut");
			pJaneDoeState->Set_Bool("OutReserve", false);
		}
	}

	__super::Update(pOwner, dt);
}

void CJaneDoeState_CounterAttack::Exit(CJaneDoe* pOwner)
{
	pOwner->Pop_Invincible();
	__super::Exit(pOwner);
}

void CJaneDoeState_Counter_01::Enter(CJaneDoe* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter_01")
		.Speed(1.2f)
		.Apply();
}

void CJaneDoeState_Counter_01::Update(CJaneDoe* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
		ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Counter_02::Enter(CJaneDoe* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter_02")
		.Speed(1.2f)
		.Apply();
}

void CJaneDoeState_Counter_02::Update(CJaneDoe* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
		ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Counter_03::Enter(CJaneDoe* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter_03")
		.Speed(1.2f)
		.Apply();
}

void CJaneDoeState_Counter_03::Update(CJaneDoe* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
		ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Counter_End::Enter(CJaneDoe* pOwner)
{
	CJaneDoeState_CounterAttack* pParent = static_cast<CJaneDoeState_CounterAttack*>(m_pParentState);
	_int iIndex = m_pOwnerStateMachine->Get_Int("CounterMode");
	m_pOwnerStateMachine->Set_Int("CounterMode", 999);
	const string arrEndAnims[3] =
	{
		pOwner->Get_Name() + "Attack_Counter_03_End",
		pOwner->Get_Name() + "Attack_Counter_02_End",
		pOwner->Get_Name() + "Attack_Counter_01_End",
	};

	pOwner->Get_Animator()->Change_Animation(arrEndAnims[iIndex])
		.Speed(1.2f)
		.Apply();
	pOwner->Unlock_Move();
}

void CJaneDoeState_Counter_End::Update(CJaneDoe* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
		ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}
