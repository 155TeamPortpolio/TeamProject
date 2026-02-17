#include "pch.h"
#include "JaneDoeState_Hit.h"
#include "JaneDoe.h"

CJaneDoeState_Hit* CJaneDoeState_Hit::Create()
{
	auto pInstance = new CJaneDoeState_Hit();
	pInstance->m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
	auto pSubStateMachine = pInstance->Get_SubStateMachine();

	pSubStateMachine->Register_State("HitNormal", CJaneDoe_HitNormal::Create());
	pSubStateMachine->Register_State("HitHard", CJaneDoe_HitHard::Create());
	pSubStateMachine->Register_State("HitKnockOut", CJaneDoe_HitKnockOut::Create());

	pSubStateMachine->Set_DefaultState("HitNormal");

	return pInstance;
}

void CJaneDoeState_Hit::Enter(CJaneDoe* pOwner)
{
	pOwner->Lock_Move();
	pOwner->Stop_Rotation();
	m_fWeight *= 0.2f;

	_int iEntryMode = pOwner->Get_StateMachine()->Get_Int("HitEntryMode");
	pOwner->Get_StateMachine()->Set_Int("HitEntryMode", 0);

	switch (iEntryMode)
	{
	case 1:
		m_pSubStateMachine->Set_DefaultState("HitHard");
		break;
	case 4:
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
	if (m_pSubStateMachine->Get_CurrentStateName() == "HitNormal")
	{
		m_fWeight += dt * 0.5f;
		m_fWeight = min(m_fWeight, 1.f);
		pOwner->Get_StateMachine()->Set_Float("MoveWeight", m_fWeight);	// 디버그 용

		CCharacter::ROOTMOTION_DESC desc;
		desc.iModeMask = ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
			ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION);
		desc.fMoveWeight = m_fWeight;
		pOwner->Process_RootMotion(dt, desc);
	}
	else
	{
		pOwner->Process_RootMotion(dt, ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
			ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
	}

	if(m_pSubStateMachine->Get_CurrentState()->Get_AnimProgress() > 0.3f)
		pOwner->Unlock_Move();

	auto pStateMachine = pOwner->Get_StateMachine();
	if (pStateMachine->Get_Trigger("ToHit")
		|| Is_AnimEnd())
		pStateMachine->Set_Trigger("ToIdle");

	__super::Update(pOwner, dt);
}

void CJaneDoeState_Hit::Exit(CJaneDoe* pOwner)
{
	pOwner->Set_ResetMove(true);
	pOwner->Unlock_Move();
	__super::Exit(pOwner);
}

void CJaneDoe_HitNormal::Enter(CJaneDoe* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_L_Back" : "Hit_L_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.4f)
		.Apply();
}

void CJaneDoe_HitHard::Enter(CJaneDoe* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_H_Back" : "Hit_H_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.6f)
		.Apply();
}

void CJaneDoe_HitKnockOut::Enter(CJaneDoe* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "HitFly_Back" : "HitFly_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.7f)
		.Apply();
}
