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

	pOwner->Get_Component<CAnimator3D>()->Set_Animation(0, 10);
}

void CTestState_Dash::Update(CTestObject* pOwner, _float dt)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	if (pAnimator->Get_CurAnimDuration() > 0.95f)
	{
		pOwner->Get_StateMachine()->Set_Bool("DashFinished", true);
	}
}

void CTestState_Dash::Exit(CTestObject* pOwner)
{
	pOwner->Get_StateMachine()->Set_Bool("DashFinished", false);
}

bool CTestState_Dash::Handle_Transition(const string& strState)
{
	return false;
}
