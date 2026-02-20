#include "pch.h"
#include "MiyabiState_AssaultAttack.h"
#include "BattleSystem.h"
#include "CharacterController.h"
#include "BattlePlayer.h"
#include "Miyabi.h"

CMiyabiState_AssaultAttack* CMiyabiState_AssaultAttack::Create()
{
	auto pInstance = new CMiyabiState_AssaultAttack();
	pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
	auto pSubStateMachine = pInstance->Get_SubStateMachine();

	pSubStateMachine->Register_State("AssaultAid_Start", CMiyabiState_Assault_Start::Create());
	pSubStateMachine->Register_State("AssaultAid_End", CMiyabiState_Assault_End::Create());

	pSubStateMachine->Get_State("AssaultAid_End")->Set_Tag("End");

	pSubStateMachine->Register_Transition("AssaultAid_Start", "AssaultAid_End",
		CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

	pSubStateMachine->Set_DefaultState("AssaultAid_Start");



	return pInstance;
}

void CMiyabiState_AssaultAttack::Enter(CMiyabi* pOwner)
{
	m_pSubStateMachine->Set_Bool("ReserveNormal", false);
	pOwner->Set_WeaponEffectMesh(true);
	pOwner->Lock_Move();
	pOwner->Push_Invincible();
	__super::Enter(pOwner);
}

void CMiyabiState_AssaultAttack::Update(CMiyabi* pOwner, _float dt)
{
	auto pMiyabiState = pOwner->Get_StateMachine();
	if (pMiyabiState->Get_Bool("OutReserve"))
	{
		if (m_pSubStateMachine->Get_CurrentStateName() == "AssaultAid_End" ||
			Is_AnimEnd())
		{
			pMiyabiState->Set_Trigger("SwitchOut");
			pMiyabiState->Set_Bool("OutReserve", false);
		}
	}

	if (m_pSubStateMachine->Get_Bool("ReserveNormal"))
	{
		if (m_pSubStateMachine->Get_CurrentStateName() == "AssaultAid_End" &&
			m_fAnimProgress > 0.3f)
		{
			m_pSubStateMachine->Set_Bool("ReserveNormal", false);
			m_pParentState->Get_SubStateMachine()->Set_Int("ComboEntryIndex", 3);
			m_pParentState->Get_SubStateMachine()->Set_Trigger("ToNormalAttack");
			return;
		}
	}

	__super::Update(pOwner, dt);
}

void CMiyabiState_AssaultAttack::Exit(CMiyabi* pOwner)
{
	pOwner->Set_LookTarget(true);
	pOwner->Reset_ReserveCombo();
	pOwner->Pop_Invincible();
	pOwner->Unlock_Move();
	__super::Exit(pOwner);
}

void CMiyabiState_Assault_Start::Enter(CMiyabi* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_AssaultAid")
		.ReserveSpeed(0.f, 0.18f, 0.7f, EaseType::InQuad)
		.ReserveSpeed(0.18f, 0.32f, 3.f, EaseType::OutExpo)
		.ReserveSpeed(0.32f, 0.5f, 2.f, EaseType::Linear)
		.ReserveSpeed(0.5f, 0.55f, 0.5f, EaseType::OutExpo)
		.ReserveSpeed(0.55f, 0.8f, 2.5f, EaseType::OutQuad)
		.ReserveSpeed(0.8f, 1.f, 0.7f, EaseType::OutQuart)
		.Apply();

	m_iMask = pOwner->Get_CCT()->Get_CollisionMask();
	pOwner->Get_CCT()->Set_CollisionMask(m_iMask - ENUM(COLLISION_GROUP::MONSTER));
	m_pOwnerStateMachine->Set_Bool("Penetrate", true);

	pOwner->Rush_Target();
	pOwner->Set_LookTarget(false);

	m_vPos = pOwner->Get_WorldPos();
	m_vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
}

void CMiyabiState_Assault_Start::Update(CMiyabi* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
		ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

	for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
	{
		if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

		if (Event.Tag == "AreaAttack")
		{
			BattleSystem()->TakeAreaDamage(m_vPos + m_vLook * 2.f, 3.f, HitDesc()
				.Name(pOwner->Get_CharacterName())
				.Type(HIT_TYPE::ONCE)
				.Damage(pOwner->Get_AttackPower() * 0.375f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::NORMAL)
			);
		}
		else if (Event.Tag == "AreaAttackHard")
		{
			BattleSystem()->TakeAreaDamage(m_vPos + m_vLook * 2.f, 3.f, HitDesc()
				.Name(pOwner->Get_CharacterName())
				.Type(HIT_TYPE::ONCE)
				.Damage(pOwner->Get_AttackPower() * 0.375f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::HARD)
			);
		}
	}

	Update_Effects(pOwner);
}

void CMiyabiState_Assault_Start::Exit(CMiyabi* pOwner)
{
	if (m_pOwnerStateMachine->Get_Bool("Penetrate"))
	{
		pOwner->Get_CCT()->Set_CollisionMask(m_iMask);
	}
}

void CMiyabiState_Assault_Start::Update_Effects(CMiyabi* pOwner)
{
	if (IsCrossAnimProgress(0.2f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash1", _vector3(0.f, 1.f, 0.f), _quaternion(-0.27f, -0.45f, -0.26f, 0.81f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting0", _vector3(-6.f, -0.6f, 1.6f), _quaternion(0.99f, 0.12f, -0.09f, -0.02f), false);
	}
	if (IsCrossAnimProgress(0.25f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash2", _vector3(0.f, 1.f, 0.f), _quaternion(0.89f, -0.03f, -0.04f, -0.45f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting1", _vector3(-0.7f, 1.4f, -5.4f), _quaternion(-0.26f, -0.57f, -0.25f, 0.74f), false);
	}
	if (IsCrossAnimProgress(0.3f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash3", _vector3(0.f, 1.f, 0.f), _quaternion(-0.47f, -0.31f, -0.52f, 0.64f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting2", _vector3(-2.7f, -2.6f, -5.5f), _quaternion(-0.15f, -0.5f, -0.25f, 0.82f), false);
	}
	if (IsCrossAnimProgress(0.35f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash0", _vector3(0.f, 1.f, 0.f), _quaternion(0.37f, 0.59f, -0.34f, 0.63f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting3", _vector3(-1.f, -0.4f, -4.2f), _quaternion(0.6f, -0.15f, 0.68f, 0.4f), false);
	}

	if (IsCrossAnimProgress(0.4f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash1", _vector3(0.f, 1.f, 0.f), _quaternion(-0.27f, -0.45f, -0.26f, 0.81f));
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
	if (IsCrossAnimProgress(0.7f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash3", _vector3(0.f, 1.f, 0.f), _quaternion(-0.47f, -0.31f, -0.52f, 0.64f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting2", _vector3(-2.7f, -2.6f, -5.5f), _quaternion(-0.15f, -0.5f, -0.25f, 0.82f), false);
	}
	if (IsCrossAnimProgress(0.75f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash0", _vector3(0.f, 1.f, 0.f), _quaternion(0.37f, 0.59f, -0.34f, 0.63f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting3", _vector3(-1.f, -0.4f, -4.2f), _quaternion(0.6f, -0.15f, 0.68f, 0.4f), false);
	}
	if (IsCrossAnimProgress(0.8f))
	{
		pOwner->Play_Effect("Miyabi_Normal1_Slash1", _vector3(0.f, 1.f, 0.f), _quaternion(-0.27f, -0.45f, -0.26f, 0.81f));
		pOwner->Play_Effect("Miyabi_Ex1_Sting0", _vector3(-6.f, -0.6f, 1.6f), _quaternion(0.99f, 0.12f, -0.09f, -0.02f), false);
	}
}

void CMiyabiState_Assault_End::Enter(CMiyabi* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_AssaultAid_End")
		.Speed(1.f)
		.Apply();
}

void CMiyabiState_Assault_End::Update(CMiyabi* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
		ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

_bool CMiyabiState_Assault_End::Handle_Transition(CMiyabi* pOwner, const string& strState)
{
	return true;
}
