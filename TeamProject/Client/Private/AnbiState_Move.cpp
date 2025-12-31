#include "pch.h"
#include "AnbiState_Move.h"
#include "AnbiState_Walk.h"
#include "AnbiState_Run.h"

#include "Anbi.h"

#include "CharacterController.h"

void CAnbiState_Move::Enter(CAnbi* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CAnbi>::Create();
        m_pSubStateMachine->Register_State("Walk", CAnbiState_Walk::Create());
        m_pSubStateMachine->Register_State("Run", CAnbiState_Run::Create());
        m_pSubStateMachine->Register_Transition("Walk", "Run",
            CStateMachine<CAnbi>::CONDITION_TRIGGER, "ToRun");
        m_pSubStateMachine->Set_DefaultState("Walk");
    }

    __super::Enter(pOwner);
}

void CAnbiState_Move::Update(CAnbi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    if (m_pSubStateMachine)
    {
        IHState<CAnbi>* pMoveType =
            dynamic_cast<IHState<CAnbi>*>(m_pSubStateMachine->Get_CurrentState());

        if (pMoveType && pMoveType->Has_SubStateMachine())
        {
            CStateMachine<CAnbi>* pAnimFSM = pMoveType->Get_SubStateMachine();
            IBaseState<CAnbi>* pCurrentAnim = pAnimFSM->Get_CurrentState();

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

_bool CAnbiState_Move::Handle_Transition(CAnbi* pOwner, const string& strState)
{
    if (strState != "Move")
    {
        if (!m_pSubStateMachine)
            return true;

        IHState<CAnbi>* pMoveType =
            dynamic_cast<IHState<CAnbi>*>(m_pSubStateMachine->Get_CurrentState());

        if (!pMoveType || !pMoveType->Has_SubStateMachine())
            return true;

        CStateMachine<CAnbi>* pAnimFSM = pMoveType->Get_SubStateMachine();
        IBaseState<CAnbi>* pCurrentAnim = pAnimFSM->Get_CurrentState();

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
