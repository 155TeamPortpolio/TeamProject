#include "pch.h"
#include "CorinState_Evade.h"
#include "Corin.h"

#include "CorinState_Dash.h"
#include "CorinState_Backstep.h"

void CCorinState_Evade::Enter(CCorin* pOwner)
{
    pOwner->Use_Evade();

    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Dash", CCorinState_Dash::Create());
        m_pSubStateMachine->Register_State("Backstep", CCorinState_Backstep::Create());

        m_pSubStateMachine->Get_State("Dash")->Set_Tag("Dash");
        m_pSubStateMachine->Get_State("Backstep")->Set_Tag("Backstep");
    }

    if (pOwner->Is_Move())
        m_pSubStateMachine->Set_DefaultState("Dash");
    else
        m_pSubStateMachine->Set_DefaultState("Backstep");
    

    __super::Enter(pOwner);
}

void CCorinState_Evade::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    IBaseState<CCorin>* pCurrent = m_pSubStateMachine->Get_CurrentState();
    if (pCurrent && pCurrent->Is_AnimEnd())
    {
        CStateMachine<CCorin>* pRootFSM = pOwner->Get_StateMachine();

        if (pCurrent->Get_Tag() == "Dash")
        {
            if (pOwner->Is_Move())
                pRootFSM->Set_Int("MoveEntryMode", 2); // Run_Loop
            else
                pRootFSM->Set_Int("MoveEntryMode", 1); // Run_End

            pRootFSM->Set_Trigger("ToMove");
        }
        else
        {
            pRootFSM->Set_Trigger("ToIdle");
        }
    }
}

void CCorinState_Evade::Exit(CCorin* pOwner)
{
}
