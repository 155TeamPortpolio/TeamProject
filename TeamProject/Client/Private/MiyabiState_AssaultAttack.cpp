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
	//pOwner->Rush_Target();
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_AssaultAid")
		.Speed(1.f)
		.Apply();

	m_iMask = pOwner->Get_CCT()->Get_CollisionMask();
	pOwner->Get_CCT()->Set_CollisionMask(m_iMask - ENUM(COLLISION_GROUP::MONSTER));
	pOwner->Set_LookTarget(false);
	m_pOwnerStateMachine->Set_Bool("Penetrate", true);

	m_vPos = pOwner->Get_WorldPos();
	m_vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
}

void CMiyabiState_Assault_Start::Update(CMiyabi* pOwner, _float dt)
{
	//CCharacter::ROOTMOTION_DESC desc;
	//desc.iModeMask = ENUM(CMiyabi::ROOTMOTION_MASK::MOVE)
	//	| ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION);
	//desc.fMoveWeight = 0.5f;
	//pOwner->Process_RootMotion(dt, desc);
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
	}
}

void CMiyabiState_Assault_Start::Exit(CMiyabi* pOwner)
{
	if (m_pOwnerStateMachine->Get_Bool("Penetrate"))
	{
		pOwner->Get_CCT()->Set_CollisionMask(m_iMask);
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
