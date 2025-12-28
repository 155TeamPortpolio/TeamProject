#include "pch.h"
#include "MiyabiState_Move.h"
#include "MiyabiState_Walk.h"
#include "MiyabiState_Run.h"
#include "Miyabi.h"

void CMiyabiState_Move::Enter(CMiyabi* pOwner)
{
    if (!m_pSubStateMachine)  // 한 번만 생성
    {
        m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
        m_pSubStateMachine->Register_State("Walk", CMiyabiState_Walk::Create());
        m_pSubStateMachine->Register_State("Run", CMiyabiState_Run::Create());
        m_pSubStateMachine->Register_Transition("Walk", "Run",
            CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "WalkFinish");
        m_pSubStateMachine->Set_DefaultState("Walk");
    }

    __super::Enter(pOwner);
}

void CMiyabiState_Move::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CMiyabiState_Move::Exit(CMiyabi* pOwner)
{
}

_bool CMiyabiState_Move::Handle_Transition(CMiyabi* pOwner, const string& strState)
{
    if (strState != "Move")
    {
        if (!m_pSubStateMachine)
            return true;

        IHState<CMiyabi>* pMoveType =
            dynamic_cast<IHState<CMiyabi>*>(m_pSubStateMachine->Get_CurrentState());

        if (!pMoveType || !pMoveType->Has_SubStateMachine())
            return true;

        CStateMachine<CMiyabi>* pAnimFSM = pMoveType->Get_SubStateMachine();
        IBaseState<CMiyabi>* pCurrentAnim = pAnimFSM->Get_CurrentState();

        if (!pCurrentAnim)
            return true;

        if (pCurrentAnim->Get_Tag() != "End")
            return false;

        if (pCurrentAnim->Is_AnimEnd())
            return true;

        if (strState == "Attack")
            return true;

        return false;
    }

    return true;
}
