#include "pch.h"
#include "GameInstance.h"
#include "CorinState_Hit.h"
#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Hit::Enter(CCorin* pOwner)
{
	pOwner->Lock_Move();
	pOwner->Stop_Rotation();

	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CCorin>::Create();
		m_pSubStateMachine->Register_State("HitNormal", CCorin_HitNormal::Create());
		m_pSubStateMachine->Register_State("HitHard", CCorin_HitHard::Create());
		m_pSubStateMachine->Register_State("HitKnockOut", CCorin_HitKnockOut::Create());

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
}

void CCorinState_Hit::Update(CCorin* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

	__super::Update(pOwner, dt);
}

void CCorinState_Hit::Exit(CCorin* pOwner)
{
	pOwner->Unlock_Move();
}

void CCorin_HitNormal::Enter(CCorin* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_L_Back" : "Hit_L_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Speed(1.5f)
		.EndAt(0.9f)
		.Apply();
}

void CCorin_HitHard::Enter(CCorin* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "Hit_H_Back" : "Hit_H_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Apply();
}

void CCorin_HitKnockOut::Enter(CCorin* pOwner)
{
	string strAnim = pOwner->Get_Name();
	strAnim += pOwner->Get_StateMachine()->Get_Bool("IsBehind") ? "HitFly_Back" : "HitFly_Front";
	pOwner->Get_Animator()->Change_Animation(strAnim)
		.Apply();
}
