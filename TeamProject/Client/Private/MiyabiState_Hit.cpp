#include "pch.h"
#include "MiyabiState_Hit.h"
#include "Miyabi.h"

CMiyabiState_Hit* CMiyabiState_Hit::Create()
{
	auto pInstance = new CMiyabiState_Hit();
	pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
	auto pSubStateMachine = pInstance->Get_SubStateMachine();

	pSubStateMachine->Register_State("HitNormal", CMiyabi_HitNormal::Create());
	pSubStateMachine->Register_State("HitHard", CMiyabi_HitHard::Create());
	pSubStateMachine->Register_State("HitKnockOut", CMiyabi_HitKnockOut::Create());

	pSubStateMachine->Set_DefaultState("HitNormal");

	return pInstance;
}

void CMiyabiState_Hit::Enter(CMiyabi* pOwner)
{
	pOwner->Lock_Move();
	pOwner->Stop_Rotation();
	m_fWeight *= 0.2f;

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
	m_fWeight += dt * 0.5f;
	m_fWeight = min(m_fWeight, 1.f);
	pOwner->Get_StateMachine()->Set_Float("MoveWeight", m_fWeight);	// 디버그 용

	CCharacter::ROOTMOTION_DESC desc;
	desc.iModeMask = ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
		ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION);
	desc.fMoveWeight = m_fWeight;
	pOwner->Process_RootMotion(dt, desc);

	if (m_pSubStateMachine->Get_CurrentState()->Get_AnimProgress() > 0.3f)
		pOwner->Unlock_Move();

	auto pStateMachine = pOwner->Get_StateMachine();
	if ( pStateMachine->Get_Trigger("ToHit")
		|| Is_AnimEnd())
		pStateMachine->Set_Trigger("ToIdle");

	__super::Update(pOwner, dt);
}

void CMiyabiState_Hit::Exit(CMiyabi* pOwner)
{
	pOwner->Set_ResetMove(true);
	pOwner->Unlock_Move();
	__super::Exit(pOwner);
}

void CMiyabi_HitNormal::Enter(CMiyabi* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_L_Back" : "Hit_L_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.4f)
		.Apply();
}

void CMiyabi_HitHard::Enter(CMiyabi* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_H_Back" : "Hit_H_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.6f)
		.Apply();
}

void CMiyabi_HitKnockOut::Enter(CMiyabi* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "HitFly_Back" : "HitFly_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.7f)
		.Apply();
}
