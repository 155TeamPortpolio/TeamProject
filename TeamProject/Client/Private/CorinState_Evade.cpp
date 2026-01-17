#include "pch.h"
#include "BattleSystem.h"
#include "CorinState_Evade.h"
#include "Corin.h"

#include "CorinState_Dash.h"
#include "CorinState_Backstep.h"

void CCorinState_Evade::Enter(CCorin* pOwner)
{
    pOwner->Push_Invincible();

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
    
    m_pSubStateMachine->Set_Bool("Extreme", false);

    __super::Enter(pOwner);
}

void CCorinState_Evade::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_fAnimProgress >= 0.1f)
    {
        if (pOwner->Can_Evade() && !m_pSubStateMachine->Get_Bool("Extreme"))
        {
            BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::EVADE);
            m_pSubStateMachine->Set_Bool("Extreme", true);
        }
    }

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        _int iExitMode = m_pSubStateMachine->Get_Int("ExitMode");
        m_pSubStateMachine->Reset_Trigger("Complete");
        CStateMachine<CCorin>* pRootFSM = pOwner->Get_StateMachine();
        
        switch (iExitMode)
        {
        case 4:
            pRootFSM->Set_Int("IdleEntryMode", 1);
            pRootFSM->Set_Trigger("ToIdle");
            break;
        case 3: // RushAttack
            pRootFSM->Set_Int("AttackEntryMode", 1);
            pRootFSM->Set_Trigger("Attack");
            break;
        case 2: // Run
            pRootFSM->Set_Int("MoveEntryMode", 2);
            pRootFSM->Set_Trigger("ToMove");
            break;
        default: // Idle
            pRootFSM->Set_Trigger("ToIdle");
            break;
        }
    }
}

void CCorinState_Evade::Exit(CCorin* pOwner)
{
    pOwner->Pop_Invincible();
}