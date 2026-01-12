#include "pch.h"
#include "OfficeMeowState_Welcome.h"
#include "OfficeMeow.h"

#include "Helper_Func.h"

void COfficeMeowState_Welcome::Enter(COfficeMeow* pOwner)
{
    _int num = Helper::Get_Random_Int(1, 3);

    auto pAnimator = pOwner->Get_Component<CAnimator3D>();

    string strAnim = "Welcome0" + to_string(num);
    pAnimator->Change_Animation(pOwner->Get_AnimName() + strAnim)
        .Loop(false)
        .Apply();
}

void COfficeMeowState_Welcome::Update(COfficeMeow* pOwner, _float dt)
{
    auto pAnimator = pOwner->Get_Component<CAnimator3D>();

    if (m_fAnimProgress >= 0.95f)
    {
        CStateMachine<COfficeMeow>* pRootFSM = pOwner->Get_StateMachine();
        pRootFSM->Set_Trigger("ToIdle");
    }
}