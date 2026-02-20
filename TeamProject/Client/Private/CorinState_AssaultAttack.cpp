#include "pch.h"
#include "CorinState_AssaultAttack.h"

#include "GameInstance.h"
#include "EventSystem.h"
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
			m_pParentState->Get_SubStateMachine()->Set_Int("ComboEntryIndex", 3);
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
				.Damage(pOwner->Get_AttackPower() * 0.684f * Helper::Get_Random_Float(1.f,1.5f)
					, DAMAGE_TYPE::HARD)
				.Interval(0.05f)
			);
		}
		else if (Event.Tag == "SawIntervalHard")
		{
			pOwner->Begin_AttackCollider("Saw",
				HitDesc()
				.Type(HIT_TYPE::INTERVAL)
				.Damage(pOwner->Get_AttackPower() * 0.684f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::HARD)
				.Interval(0.1f)
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

	pOwner->Play_Effect("Corin_Saw_Slash0", _vector3(0.f, 0.f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f));
}

void CCorinState_Assault_Start::Update(CCorin* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

	if (IsCrossAnimProgress(0.5f))
	{
		if (pOwner->Get_CurrentTutorial() == TUTORIAL_TYPE::EXTREME_SUPPORT)
		{
			TUTORIAL_ACTION_DESC desc;
			desc.eAction = TUTORIAL_ACTION::ASSIST_CHARGE;
			EventSystem()->Broadcast<TUTORIAL_ACTION_DESC>(desc);
		}
	}
	Update_Effects(pOwner);
}

void CCorinState_Assault_Start::Update_Effects(CCorin* pOwner)
{
	if (IsCrossAnimProgress(0.08f))
		pOwner->Play_Effect("Corin_Normal_Slash0", _vector3(0.f, 1.f, 0.f), _quaternion(0.09f, -0.67f, 0.11f, 0.73f));
	if (IsCrossAnimProgress(0.2f))
		pOwner->Play_Effect("Corin_Normal_Slash1", _vector3(0.f, 1.f, 0.f), _quaternion(0.19f, 0.68f, 0.11f, -0.7f));
	if (IsCrossAnimProgress(0.32f))
		pOwner->Play_Effect("Corin_Normal_Slash2", _vector3(0.f, 1.f, 0.f), _quaternion(-0.07f, -0.56f, 0.17f, 0.81f));
	if (IsCrossAnimProgress(0.49f))
		pOwner->Play_Effect("Corin_Normal_Slash3", _vector3(0.f, 1.f, 0.f), _quaternion(0.67f, 0.3f, 0.63f, 0.27f));
}

void CCorinState_Assault_End::Enter(CCorin* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "AssaultAid_End")
		.Speed(1.2f)
		.Apply();

	pOwner->Stop_Effect("Corin_Saw_Slash0");
	pOwner->Play_Effect("Corin_Assault_Explode", _vector3(0.f, 0.f, 1.5f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
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
