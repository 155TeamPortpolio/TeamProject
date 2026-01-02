#include "pch.h"
#include "CorinState_Move.h"
#include "CorinState_Walk.h"
#include "CorinState_Run.h"

#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Move::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Walk", CCorinState_Walk::Create());
        m_pSubStateMachine->Register_State("Run", CCorinState_Run::Create());
        m_pSubStateMachine->Register_Transition("Walk", "Run",
            CStateMachine<CCorin>::CONDITION_TRIGGER, "ToRun");
        m_pSubStateMachine->Set_DefaultState("Walk");
    }

    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("MoveEntryMode");
    pOwner->Get_StateMachine()->Set_Int("MoveEntryMode", 0);

    switch (iEntryMode)
    {
    case 2:
        m_pSubStateMachine->Set_DefaultState("Run");
        break;
    case 1:
        m_pSubStateMachine->Set_DefaultState("Run");
        m_pSubStateMachine->Set_Trigger("SkipToEnd");
        break;
    default:
        m_pSubStateMachine->Set_DefaultState("Walk");
        break;
    }

    __super::Enter(pOwner);
}

void CCorinState_Move::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    if (m_pSubStateMachine)
    {
        IHState<CCorin>* pMoveType =
            dynamic_cast<IHState<CCorin>*>(m_pSubStateMachine->Get_CurrentState());

        if (pMoveType && pMoveType->Has_SubStateMachine())
        {
            CStateMachine<CCorin>* pAnimFSM = pMoveType->Get_SubStateMachine();
            IBaseState<CCorin>* pCurrentAnim = pAnimFSM->Get_CurrentState();

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

_bool CCorinState_Move::Handle_Transition(CCorin* pOwner, const string& strState)
{
    if (strState == "Evade")
        return true;

    if (strState != "Move")
    {
        if (!m_pSubStateMachine)
            return true;

        IHState<CCorin>* pMoveType =
            dynamic_cast<IHState<CCorin>*>(m_pSubStateMachine->Get_CurrentState());

        if (!pMoveType || !pMoveType->Has_SubStateMachine())
            return true;

        CStateMachine<CCorin>* pAnimFSM = pMoveType->Get_SubStateMachine();
        IBaseState<CCorin>* pCurrentAnim = pAnimFSM->Get_CurrentState();

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
