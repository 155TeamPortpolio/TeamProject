#include "pch.h"
#include "MiyabiState_Move.h"
#include "MiyabiState_Walk.h"
#include "MiyabiState_Run.h"
#include "Miyabi.h"

#include "CharacterController.h"

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

    // Walk/Run의 End 상태를 Move의 AnimProgress에 반영
    if (m_pSubStateMachine)
    {
        IHState<CMiyabi>* pMoveType =
            dynamic_cast<IHState<CMiyabi>*>(m_pSubStateMachine->Get_CurrentState());

        if (pMoveType && pMoveType->Has_SubStateMachine())
        {
            CStateMachine<CMiyabi>* pAnimFSM = pMoveType->Get_SubStateMachine();
            IBaseState<CMiyabi>* pCurrentAnim = pAnimFSM->Get_CurrentState();

            if (pCurrentAnim && pCurrentAnim->Get_Tag() == "End")
            {
                if (pCurrentAnim->Is_AnimEnd() || pCurrentAnim->Get_AnimProgress() >= 1.f)
                {
                    m_fAnimProgress = 1.f;
                }
                else
                {
                    m_fAnimProgress = 0.f;
                }
            }
            else
            {
                m_fAnimProgress = 0.f;
            }
        }
    }
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

        // End 상태가 아니면 거부
        if (pCurrentAnim->Get_Tag() != "End")
            return false;

        // Idle로만 전환 허용
        if (strState == "Idle")
        {
            // 애니메이션 끝났거나 새로운 입력이 있으면 허용
            if (pCurrentAnim->Is_AnimEnd() || pOwner->Is_Input())
                return true;
        }

        // Idle 외의 다른 상태로는 직접 전환 불가
        return false;
    }

    return true;
}