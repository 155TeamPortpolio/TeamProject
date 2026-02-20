#include "pch.h"
#include "JaneDoeState_AssaultAttack.h"

#include "GameInstance.h"
#include "EventSystem.h"
#include "BattleSystem.h"

#include "JaneDoe.h"

CJaneDoeState_AssaultAttack* CJaneDoeState_AssaultAttack::Create()
{
	auto pInstance = new CJaneDoeState_AssaultAttack();
	pInstance->m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
	auto pSubStateMachine = pInstance->Get_SubStateMachine();

	pSubStateMachine->Register_State("AssaultAid_Start", CJaneDoeState_Assault_Start::Create());
	pSubStateMachine->Register_State("AssaultAid_End", CJaneDoeState_Assault_End::Create());

	pSubStateMachine->Get_State("AssaultAid_End")->Set_Tag("End");

	pSubStateMachine->Register_Transition("AssaultAid_Start", "AssaultAid_End",
		CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

	pSubStateMachine->Set_DefaultState("AssaultAid_Start");

	pSubStateMachine->Set_Bool("ReserveNormal", false);

	return pInstance;
}


void CJaneDoeState_AssaultAttack::Enter(CJaneDoe* pOwner)
{
	pOwner->Lock_Move();
	pOwner->Push_Invincible();

	__super::Enter(pOwner);
}

void CJaneDoeState_AssaultAttack::Update(CJaneDoe* pOwner, _float dt)
{
	// 타격 이벤트
	for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
	{
		if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

		if (Event.Tag == "AreaAttack")
		{
			_vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
			_vector3 vPos = pOwner->Get_WorldPos();
			BattleSystem()->TakeAreaDamage(vPos + vLook * 2.f, 2.f, HitDesc()
				.Name(pOwner->Get_CharacterName())
				.Type(HIT_TYPE::ONCE)
				.Damage(pOwner->Get_AttackPower() * 0.575f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::NORMAL)
			);
		}
		if (Event.Tag == "AreaAttackHard")
		{
			_vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
			_vector3 vPos = pOwner->Get_WorldPos();
			BattleSystem()->TakeAreaDamage(vPos + vLook * 2.f, 2.f, HitDesc()
				.Name(pOwner->Get_CharacterName())
				.Type(HIT_TYPE::ONCE)
				.Damage(pOwner->Get_AttackPower() * 0.575f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::HARD)
			);
		}
		else if (Event.Tag == "LFootStart")
		{
			pOwner->Begin_AttackCollider("FootWeapon_L", HitDesc()
				.Type(HIT_TYPE::ONCE)
				.Damage(pOwner->Get_AttackPower() * 0.575f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::NORMAL)
			);
		}
		else if (Event.Tag == "LFootEnd")
		{
			pOwner->End_AttackCollider("FootWeapon_L");
		}
		else if (Event.Tag == "RFootStart")
		{
			pOwner->Begin_AttackCollider("FootWeapon_R", HitDesc()
				.Type(HIT_TYPE::ONCE)
				.Damage(pOwner->Get_AttackPower() * 0.575f * Helper::Get_Random_Float(1.f, 1.5f)
					, DAMAGE_TYPE::NORMAL)
			);
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

void CJaneDoeState_AssaultAttack::Exit(CJaneDoe* pOwner)
{
	pOwner->Reserve_ComboAttack();
	pOwner->Pop_Invincible();
	pOwner->Unlock_Move();
	__super::Exit(pOwner);
}

void CJaneDoeState_Assault_Start::Enter(CJaneDoe* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_AssaultAid")
		.Speed(1.5f)
		.ReserveSpeed(0.f, 0.6f, 1.f, EaseType::InCubic)
		.ReserveSpeed(0.6f, 0.99f, 2.f, EaseType::OutQuart)
		.Apply();
}

void CJaneDoeState_Assault_Start::Update(CJaneDoe* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
		ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

	if (IsCrossAnimProgress(0.32f))
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

void CJaneDoeState_Assault_Start::Update_Effects(CJaneDoe* pOwner)
{
	if (IsCrossAnimProgress(0.05f))
		pOwner->Play_Effect("JaneDoe_Normal_Slash0", _vector3(0.f, 1.4f, 0.f), _quaternion(0.55f, -0.49f, -0.67f, -0.11f));
	if (IsCrossAnimProgress(0.12f))
		pOwner->Play_Effect("JaneDoe_Normal_Slash1", _vector3(0.f, 1.7f, 0.f), _quaternion(-0.11f, 0.7f, 0.7f, 0.11f));
	if (IsCrossAnimProgress(0.25f))
		pOwner->Play_Effect("JaneDoe_Wide_Slash", _vector3(0.f, 0.2f, 2.3f), _quaternion(0.f, 0.74f, 0.68f, 0.f), false);
}

void CJaneDoeState_Assault_End::Enter(CJaneDoe* pOwner)
{
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_AssaultAid_End")
		.Speed(1.2f)
		.Apply();
}

void CJaneDoeState_Assault_End::Update(CJaneDoe* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
		ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

_bool CJaneDoeState_Assault_End::Handle_Transition(CJaneDoe* pOwner, const string& strState)
{
	return true;
}
