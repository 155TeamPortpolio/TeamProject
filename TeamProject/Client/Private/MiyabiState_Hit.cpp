#include "pch.h"
#include "MiyabiState_Hit.h"
#include "Miyabi.h"

void CMiyabiState_Hit::Enter(CMiyabi* pOwner)
{
	pOwner->Push_Invincible();
	pOwner->Lock_Move();
	pOwner->Stop_Rotation();

	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
		m_pSubStateMachine->Register_State("HitNormal", CMiyabi_HitNormal::Create());
		m_pSubStateMachine->Register_State("HitHard", CMiyabi_HitHard::Create());
		m_pSubStateMachine->Register_State("HitKnockOut", CMiyabi_HitKnockOut::Create());

		m_pSubStateMachine->Set_DefaultState("HitNormal");
	}

	_int iEntryMode = pOwner->Get_StateMachine()->Get_Int("HitEntryMode");
	pOwner->Get_StateMachine()->Set_Int("HitEntryMode", 0);

	switch (iEntryMode)
	{
	case 2:
		m_pSubStateMachine->Set_DefaultState("HitHard");
		break;
	case 3:
		m_pSubStateMachine->Set_DefaultState("HitKnockOut");
		break;
	default:
		m_pSubStateMachine->Set_DefaultState("HitNormal");
		break;
	}

	_bool IsBehind = false;
	{
		auto pTransform = pOwner->Get_Component<CTransform>();
		_vector3 vPos = pTransform->Dir(STATE::POSITION);
		_vector3 vLook = pTransform->Dir(STATE::LOOK);
		_vector3 vAttackerPos = pOwner->Get_HitTargetPos();
		_vector3 vDir = vPos - vAttackerPos;
		vDir.y = 0.f;
		vDir.Normalize();
		IsBehind = vLook.Dot(vDir) > 0.f;
	}
	pOwner->Get_StateMachine()->Set_Bool("IsBehind", IsBehind);
	__super::Enter(pOwner);
}

void CMiyabiState_Hit::Update(CMiyabi* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
		ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

	if (m_pSubStateMachine->Get_CurrentState()->Get_AnimProgress() > 0.3f)
		pOwner->Unlock_Move();

	if (Is_AnimEnd())
		pOwner->Get_StateMachine()->Set_Trigger("ToIdle");

	__super::Update(pOwner, dt);
}

void CMiyabiState_Hit::Exit(CMiyabi* pOwner)
{
	pOwner->Unlock_Move();
	pOwner->Pop_Invincible();
	__super::Exit(pOwner);
}

void CMiyabi_HitNormal::Enter(CMiyabi* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_L_Back" : "Hit_L_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.8f)
		.Apply();
}

void CMiyabi_HitHard::Enter(CMiyabi* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_H_Back" : "Hit_H_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Apply();
}

void CMiyabi_HitKnockOut::Enter(CMiyabi* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "HitFly_Back" : "HitFly_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Apply();
}
