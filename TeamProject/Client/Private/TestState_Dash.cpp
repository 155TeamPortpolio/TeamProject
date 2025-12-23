#include "pch.h"
#include "TestState_Dash.h"
#include "TestObject.h"
#include "Animator3D.h"
#include "CharacterController.h"

void CTestState_Dash::Enter(CTestObject* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation(10)
		.Apply();
}

void CTestState_Dash::Update(CTestObject* pOwner, _float dt)
{
	_vector3 vDir = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK) * -1.f;
	vDir.Normalize();

    if (m_fStateTime < 0.5)
    {
        _float fRatio = 1.f - (m_fStateTime / 0.5);         // Easing
        _float fCurSpeed = pOwner->Get_Speed() * fRatio;

        pOwner->Get_Component<CCharacterController>()->Move_Direction(
            vDir, fCurSpeed, dt);
    }
    else
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
	if (strState == "Idle" || strState == "Walk")
		return true;

	return false;
}
