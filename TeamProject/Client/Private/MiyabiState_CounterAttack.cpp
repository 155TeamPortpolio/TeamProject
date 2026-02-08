#include "pch.h"
#include "MiyabiState_CounterAttack.h"
#include "Miyabi.h"

#include "BattleSystem.h"

#include "CharacterController.h"

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
	m_pSubStateMachine->Set_Bool("ReserveNormal", false);
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

	if (m_pSubStateMachine->Get_CurrentStateName() == "Counter_Start" &&
		m_pSubStateMachine->Get_Bool("ReserveNormal") &&
		Is_AnimEnd())
	{
		m_pSubStateMachine->Set_Bool("ReserveNormal", false);
		m_pParentState->Get_SubStateMachine()->Set_Trigger("ToNormalAttack");
		return;
	}

	__super::Update(pOwner, dt);
}

void CMiyabiState_CounterAttack::Exit(CMiyabi* pOwner)
{
	pOwner->Set_LookTarget(true);
	pOwner->Reset_ReserveCombo();
	pOwner->Pop_Invincible();
	pOwner->Unlock_Move();
	__super::Exit(pOwner);
}

void CMiyabiState_Counter_Start::Enter(CMiyabi* pOwner)
{
	pOwner->Rush_Target();
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter")
		.Speed(1.f)
		.EndAt(0.9f)
		.Apply();

	m_iMask = pOwner->Get_CCT()->Get_CollisionMask();
	pOwner->Get_CCT()->Set_CollisionMask(m_iMask - ENUM(COLLISION_GROUP::MONSTER));
	pOwner->Set_LookTarget(false);
	m_pOwnerStateMachine->Set_Bool("Penetrate", true);

	m_vPos = pOwner->Get_WorldPos();
	m_vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
}

void CMiyabiState_Counter_Start::Update(CMiyabi* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
		ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

	for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
	{
		if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
		if (Event.Tag == "KatanaStart")
		{
			pOwner->Begin_AttackCollider("KatanaWeapon", HitDesc()
				.Name(pOwner->Get_CharacterName())
				.Type(HIT_TYPE::ONCE)
				.Damage(pOwner->Get_AttackPower() * 0.491f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::HARD)
			);
		}
		else if (Event.Tag == "KatanaEnd")
		{
			pOwner->End_AttackCollider("KatanaWeapon");
		}
		else if (Event.Tag == "AreaAttack")
		{
			BattleSystem()->TakeAreaDamage(m_vPos + m_vLook * 2.f, 3.f, HitDesc()
				.Name(pOwner->Get_CharacterName())
				.Type(HIT_TYPE::ONCE)
				.Damage(pOwner->Get_AttackPower() * 0.491f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::HARD)
			);
		}
	}
}

void CMiyabiState_Counter_Start::Exit(CMiyabi* pOwner)
{
	if (m_pOwnerStateMachine->Get_Bool("Penetrate"))
	{
		pOwner->Get_CCT()->Set_CollisionMask(m_iMask);
	}
}

void CMiyabiState_Counter_End::Enter(CMiyabi* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter_End")
		.Speed(1.f)
		.BlendDuration(0.1f)
		.Apply();
}

void CMiyabiState_Counter_End::Update(CMiyabi* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
		ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}
