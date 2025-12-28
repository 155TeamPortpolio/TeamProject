#include "pch.h"
#include "MiyabiState_Move.h"
#include "MiyabiState_Walk.h"
#include "MiyabiState_Run.h"
#include "Miyabi.h"

void CMiyabiState_Move::Enter(CMiyabi* pOwner)
{
	m_pSubStateMachine = CStateMachine<CMiyabi>::Create();

	m_pSubStateMachine->Register_State("Walk", CMiyabiState_Walk::Create());
	m_pSubStateMachine->Register_State("Run", CMiyabiState_Run::Create());
    m_pSubStateMachine->Register_Transition("Walk", "Run",
        CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "WalkFinish");

	m_pSubStateMachine->Set_DefaultState("Walk");

    __super::Enter(pOwner);
}

void CMiyabiState_Move::Update(CMiyabi* pOwner, _float dt)
{

}

void CMiyabiState_Move::Exit(CMiyabi* pOwner)
{
}

_bool CMiyabiState_Move::Handle_Transition(CMiyabi* pOwner, const string& strState)
{
    // Move에서 나가려는 경우
    if (strState != "Move")
    {
        if (!m_pSubStateMachine)
            return true;

        // 현재 Walk 또는 Run 상태 확인
        IHState<CMiyabi>* pMoveType =
            dynamic_cast<IHState<CMiyabi>*>(m_pSubStateMachine->Get_CurrentState());

        if (!pMoveType || !pMoveType->Has_SubStateMachine())
            return true;

        // Walk/Run의 서브 상태 확인
        CStateMachine<CMiyabi>* pAnimFSM = pMoveType->Get_SubStateMachine();
        IBaseState<CMiyabi>* pCurrentAnim = pAnimFSM->Get_CurrentState();

        if (!pCurrentAnim)
            return true;

        // End 상태가 아니면 거부
        if (pCurrentAnim->Get_Tag() != "End")
            return false;

        // End 상태에서:
        // 1. 애니메이션 끝났으면 허용
        if (pCurrentAnim->Is_AnimEnd())
            return true;

        // 2. 입력(공격 등)이면 즉시 허용
        if (strState == "Attack")
            return true;

        // 그 외는 거부
        return false;
    }

    return true;
}
