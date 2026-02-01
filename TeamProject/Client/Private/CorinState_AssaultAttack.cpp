#include "pch.h"
#include "CorinState_AssaultAttack.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"
#include "Corin.h"

CCorinState_AssaultAttack* CCorinState_AssaultAttack::Create()
{
	auto pInstance = new CCorinState_AssaultAttack();
	pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
	auto pSubStateMachine = pInstance->Get_SubStateMachine();

	pSubStateMachine->Register_State("AssaultAid_Start", CCorinState_Assault_Start::Create());
	pSubStateMachine->Register_State("AssaultAid_End", CCorinState_Assault_End::Create());

	pSubStateMachine->Get_State("AssaultAid_End")->Set_Tag("Explode");

	pSubStateMachine->Register_Transition("AssaultAid_Start", "AssaultAid_End",
		CStateMachine<CCorin>::CONDITION_ANIMATION_END);

	pSubStateMachine->Set_DefaultState("AssaultAid_Start");

	pSubStateMachine->Set_Bool("ReserveNormal", false);

	return pInstance;
}

void CCorinState_AssaultAttack::Enter(CCorin* pOwner)
{
	pOwner->Lock_Move();
	pOwner->Push_Invincible();
	__super::Enter(pOwner);
}

void CCorinState_AssaultAttack::Update(CCorin* pOwner, _float dt)
{
	if (m_pSubStateMachine->Get_Bool("ReserveNormal"))
	{
		if (m_pSubStateMachine->Get_CurrentStateName() == "AssaultAid_End" &&
			m_fAnimProgress > 0.2f)
		{
			m_pSubStateMachine->Set_Bool("ReserveNormal", false);
			m_pParentState->Get_SubStateMachine()->Set_Trigger("ToNormalAttack");
			return;
		}
	}

	for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
	{
		if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
		if (Event.Tag == "SawInterval")
		{
			pOwner->Begin_AttackCollider("Saw",
				HitDesc()
				.Type(HIT_TYPE::INTERVAL)
				.Damage(pOwner->Get_AttackPower() * 5.475f * Helper::Get_Random_Float(1.f,1.5f)
					, DAMAGE_TYPE::HARD)
				.Interval(0.05f)
				.Charge(5.f, 50.f)
			);
		}
		else if (Event.Tag == "SawEnd")
		{
			pOwner->End_AttackCollider("Saw");
		}
	}

	auto pCorinState = pOwner->Get_StateMachine();
	if (pCorinState->Get_Bool("OutReserve"))
	{
		if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "Explode" ||
			Is_AnimEnd())
		{
			pCorinState->Set_Trigger("SwitchOut");
			pCorinState->Set_Bool("OutReserve", false);
		}
	}

	__super::Update(pOwner, dt);
}

void CCorinState_AssaultAttack::Exit(CCorin* pOwner)
{
	pOwner->Reset_ReserveCombo();
	pOwner->Pop_Invincible();
	pOwner->Unlock_Move();
	__super::Exit(pOwner);
}

void CCorinState_Assault_Start::Enter(CCorin* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "AssaultAid")
		.Speed(0.8f)
		.Apply();
}

void CCorinState_Assault_Start::Update(CCorin* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_Assault_End::Enter(CCorin* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "AssaultAid_End")
		.Speed(1.2f)
		.Apply();
}

void CCorinState_Assault_End::Update(CCorin* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

_bool CCorinState_Assault_End::Handle_Transition(CCorin* pOwner, const string& strState)
{
	return true;
}
