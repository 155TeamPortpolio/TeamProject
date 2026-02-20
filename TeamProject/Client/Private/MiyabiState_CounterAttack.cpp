#include "pch.h"
#include "MiyabiState_CounterAttack.h"
#include "Miyabi.h"

#include "BattleSystem.h"

#include "CharacterController.h"
#include "AudioSource.h"

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
		m_pParentState->Get_SubStateMachine()->Set_Int("ComboEntryIndex", 3);
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
	pOwner->Set_WeaponEffectMesh(true);
	pOwner->Rush_Target();
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Counter")
		.ReserveSpeed(0.f, 0.11f, 0.7f, EaseType::InCubic)
		.ReserveSpeed(0.11f, 0.25f, 2.f, EaseType::InExpo)
		.ReserveSpeed(0.25f, 0.4f, 3.f, EaseType::OutExpo)
		.ReserveSpeed(0.4f, 0.9f, 1.5f, EaseType::OutCubic)
		.EndAt(0.9f)
		.Apply();
	pOwner->Get_Component<CAudioSource>()->Slot("Miyabi_CounterAttack_SFX.wav")
		.Attribute3D(true)
		.Play();

	m_iMask = pOwner->Get_CCT()->Get_CollisionMask();
	pOwner->Get_CCT()->Set_CollisionMask(m_iMask - ENUM(COLLISION_GROUP::MONSTER));
	m_pOwnerStateMachine->Set_Bool("Penetrate", true);

	pOwner->Rush_Target();
	pOwner->Set_LookTarget(false);

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
					, DAMAGE_TYPE::NORMAL)
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
					, DAMAGE_TYPE::NORMAL)
			);
		}
		else if (Event.Tag == "AreaAttackHard")
		{
			BattleSystem()->TakeAreaDamage(m_vPos + m_vLook * 2.f, 3.f, HitDesc()
				.Name(pOwner->Get_CharacterName())
				.Type(HIT_TYPE::ONCE)
				.Damage(pOwner->Get_AttackPower() * 0.491f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::HARD)
			);
		}
	}

	Update_Effects(pOwner);
}

void CMiyabiState_Counter_Start::Exit(CMiyabi* pOwner)
{
	if (m_pOwnerStateMachine->Get_Bool("Penetrate"))
	{
		pOwner->Get_CCT()->Set_CollisionMask(m_iMask);
	}
}

void CMiyabiState_Counter_Start::Update_Effects(CMiyabi* pOwner)
{
	if (IsCrossAnimProgress(0.21f))
		pOwner->Play_Effect("Miyabi_Normal1_Slash0", _vector3(0.f, 0.8f, 0.f), _quaternion(0.23f, 0.66f, 0.68f, -0.22f));

	if (IsCrossAnimProgress(0.4f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash1", _vector3(0.f, 1.f, 0.f), _quaternion(-0.27f, -0.45f, -0.26f,0.81f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting0", _vector3(-6.f, -0.6f, 1.6f), _quaternion(0.99f, 0.12f, -0.09f, -0.02f), false);
	}
	if (IsCrossAnimProgress(0.45f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash2", _vector3(0.f, 1.f, 0.f), _quaternion(0.89f, -0.03f, -0.04f, -0.45f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting1", _vector3(-0.7f, 1.4f, -5.4f), _quaternion(-0.26f, -0.57f, -0.25f, 0.74f), false);
	}
	if (IsCrossAnimProgress(0.5f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash3", _vector3(0.f, 1.f, 0.f), _quaternion(-0.47f, -0.31f, -0.52f, 0.64f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting2", _vector3(-2.7f, -2.6f, -5.5f), _quaternion(-0.15f, -0.5f, -0.25f, 0.82f), false);
	}
	if (IsCrossAnimProgress(0.55f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash0", _vector3(0.f, 1.f, 0.f), _quaternion(0.37f, 0.59f, -0.34f, 0.63f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting3", _vector3(-1.f, -0.4f, -4.2f), _quaternion(0.6f, -0.15f, 0.68f, 0.4f), false);
	}
	if (IsCrossAnimProgress(0.6f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash1", _vector3(0.f, 1.f, 0.f), _quaternion(-0.27f, -0.45f, -0.26f, 0.81f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting0", _vector3(-6.f, -0.6f, 1.6f), _quaternion(0.99f, 0.12f, -0.09f, -0.02f), false);
	}
	if (IsCrossAnimProgress(0.65f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash2", _vector3(0.f, 1.f, 0.f), _quaternion(0.89f, -0.03f, -0.04f, -0.45f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting1", _vector3(-0.7f, 1.4f, -5.4f), _quaternion(-0.26f, -0.57f, -0.25f, 0.74f), false);
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
