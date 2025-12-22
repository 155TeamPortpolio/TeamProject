#include "pch.h"
#include "TestState_Dash.h"
#include "TestObject.h"
#include "Animator3D.h"
#include "CharacterController.h"

CTestState_Dash::CTestState_Dash()
{
}

void CTestState_Dash::Enter(CTestObject* pOwner)
{
    _vector3 vDir = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK) * -1.f;
    vDir.Normalize();

    pOwner->Get_Component<CCharacterController>()->Move_Displacement(
		vDir * m_fDashDistance, 0.f);

	pOwner->Get_Component<CAnimator3D>()->Change_Animation(10);
}

void CTestState_Dash::Update(CTestObject* pOwner, _float dt)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	if (pAnimator->Get_CurAnimIndex() == 10 && pAnimator->isCurrentAnimEnd())
	{
		pOwner->Get_StateMachine()->Set_Bool("DashFinished", true);
		pAnimator->Change_Animation(2)
			.Loop(true)
			.Apply();
	}
}

void CTestState_Dash::Exit(CTestObject* pOwner)
{
	pOwner->Get_StateMachine()->Set_Bool("DashFinished", false);
}

bool CTestState_Dash::Handle_Transition(const string& strState)
{
	if (strState == "Idle" || strState == "Walk")
		return true;

	return false;
}
