#include "pch.h"
#include "GameInstance.h"
#include "CorinState_Hit.h"
#include "Corin.h"

#include "CharacterController.h"

CCorinState_Hit* CCorinState_Hit::Create()
{
	auto pInstance = new CCorinState_Hit();
	pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
	auto pSubStateMachine = pInstance->Get_SubStateMachine();

	pSubStateMachine->Register_State("HitNormal", CCorin_HitNormal::Create());
	pSubStateMachine->Register_State("HitHard", CCorin_HitHard::Create());
	pSubStateMachine->Register_State("HitKnockOut", CCorin_HitKnockOut::Create());

	pSubStateMachine->Set_DefaultState("HitNormal");

	return pInstance;
}

void CCorinState_Hit::Enter(CCorin* pOwner)
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
	default :
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

	pOwner->Stop_Effect("Corin_Saw_Slash0");
	pOwner->Stop_Effect("Corin_Ex_Saw_Slash0");
	pOwner->Stop_Effect("Corin_Ultimate_Saw_Slash0");
}

void CCorinState_Hit::Update(CCorin* pOwner, _float dt)
{
	_float fFront = pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? -1.f : 1.f;
	CCharacter::ROOTMOTION_DESC desc;
	desc.iModeMask = ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION);
	if (m_pSubStateMachine->Get_CurrentStateName() == "HitNormal")
	{
		m_fWeight += dt * 0.5f;
		m_fWeight = min(m_fWeight, 1.f);
		pOwner->Get_StateMachine()->Set_Float("MoveWeight", m_fWeight);	// 디버그 용
		desc.fMoveWeight = m_fWeight * fFront;
	}
	else
	{
		desc.fMoveWeight = fFront;
	}
	pOwner->Process_RootMotion(dt, desc);

	if (m_pSubStateMachine->Get_CurrentState()->Get_AnimProgress() > 0.3f)
		pOwner->Unlock_Move();

	auto pStateMachine = pOwner->Get_StateMachine();
	if (pStateMachine->Get_Trigger("ToHit")
		|| Is_AnimEnd())
		pStateMachine->Set_Trigger("ToIdle");

	__super::Update(pOwner, dt);
}

void CCorinState_Hit::Exit(CCorin* pOwner)
{
	pOwner->Set_ResetMove(true);
	pOwner->Unlock_Move();
	__super::Exit(pOwner);
}

void CCorin_HitNormal::Enter(CCorin* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_L_Back" : "Hit_L_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.4f)
		.Apply();
}

void CCorin_HitHard::Enter(CCorin* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_H_Back" : "Hit_H_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.6f)
		.Apply();
}

void CCorin_HitKnockOut::Enter(CCorin* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "HitFly_Back" : "HitFly_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.7f)
		.Apply();
}
