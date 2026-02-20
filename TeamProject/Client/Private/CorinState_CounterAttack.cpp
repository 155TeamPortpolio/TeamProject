#include "pch.h"
#include "CorinState_CounterAttack.h"

#include "GameInstance.h"
#include "EventSystem.h"
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
	m_pSubStateMachine->Set_Bool("ReserveNormal", false);
	pOwner->Lock_Move();
	pOwner->Push_Invincible();
	m_eType = DAMAGE_TYPE::NORMAL;

	__super::Enter(pOwner);
}

void CCorinState_CounterAttack::Update(CCorin* pOwner, _float dt)
{
	if (IsCrossAnimProgress(0.7f))
	{
		m_eType = DAMAGE_TYPE::HARD;
	}

	for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
	{
		if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
		if (Event.Tag == "SawInterval")
		{
			pOwner->Begin_AttackCollider("Saw",
				HitDesc()
				.Type(HIT_TYPE::INTERVAL)
				.Damage(pOwner->Get_AttackPower() * 0.271f * Helper::Get_Random_Float(1.f, 1.5f)
					, m_eType)
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
		m_pParentState->Get_SubStateMachine()->Set_Int("ComboEntryIndex", 3);
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
	pOwner->Reset_ReserveCombo();
	pOwner->Pop_Invincible();
	pOwner->Unlock_Move();
	__super::Exit(pOwner);
}

void CCorinState_Counter_Start::Enter(CCorin* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter")
		.Speed(1.5f)
		.Apply();

	pOwner->Play_Effect("Corin_Saw_Slash0", _vector3(0.f, 0.f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f));
}

void CCorinState_Counter_Start::Update(CCorin* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

	if (IsCrossAnimProgress(0.7f))
	{
		if (pOwner->Get_CurrentTutorial() == TUTORIAL_TYPE::EXTREME_EVADE)
		{
			TUTORIAL_ACTION_DESC desc;
			desc.eAction = TUTORIAL_ACTION::DODGE_COUNTER;
			EventSystem()->Broadcast<TUTORIAL_ACTION_DESC>(desc);
		}
	}

	Update_Effects(pOwner);
}

void CCorinState_Counter_Start::Update_Effects(CCorin* pOwner)
{
	if (IsCrossAnimProgress(0.3f))
		pOwner->Play_Effect("Corin_Normal2_Slash0", _vector3(0.f, 0.2f, 0.f), _quaternion(0.09f, -0.67f, 0.11f, 0.73f));
}

void CCorinState_Counter_Explode::Enter(CCorin* pOwner)
{
	pOwner->End_AttackCollider("Saw");
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter_Explode")
		.Speed(1.5f)
		.Apply();

	pOwner->Stop_Effect("Corin_Saw_Slash0");
	pOwner->Play_Effect("Corin_Ex_Explode", _vector3(0.1f, 0.7f, 1.3f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
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