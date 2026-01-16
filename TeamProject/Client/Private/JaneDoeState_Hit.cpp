#include "pch.h"
#include "JaneDoeState_Hit.h"
#include "JaneDoe.h"

void CJaneDoeState_Hit::Enter(CJaneDoe* pOwner)
{
	pOwner->Push_Invincible();
	pOwner->Lock_Move();
	pOwner->Stop_Rotation();

	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
		m_pSubStateMachine->Register_State("HitNormal", CJaneDoe_HitNormal::Create());
		m_pSubStateMachine->Register_State("HitHard", CJaneDoe_HitHard::Create());
		m_pSubStateMachine->Register_State("HitKnockOut", CJaneDoe_HitKnockOut::Create());

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

void CJaneDoeState_Hit::Update(CJaneDoe* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
		ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

	__super::Update(pOwner, dt);
}

void CJaneDoeState_Hit::Exit(CJaneDoe* pOwner)
{
	pOwner->Unlock_Move();
	pOwner->Pop_Invincible();
}

void CJaneDoe_HitNormal::Enter(CJaneDoe* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_L_Back" : "Hit_L_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.8f)
		.Apply();
}

void CJaneDoe_HitHard::Enter(CJaneDoe* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_H_Back" : "Hit_H_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Apply();
}

void CJaneDoe_HitKnockOut::Enter(CJaneDoe* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "HitFly_Back" : "HitFly_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Apply();
}
