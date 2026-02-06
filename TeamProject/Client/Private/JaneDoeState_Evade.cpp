#include "pch.h"
#include "BattleSystem.h"
#include "JaneDoeState_Evade.h"
#include "JaneDoe.h"

#include "CharacterController.h"

#include "JaneDoeState_Dash.h"
#include "JaneDoeState_Backstep.h"

void CJaneDoeState_Evade::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_StateMachine()->Reset_Trigger("ToMove");
    pOwner->Get_StateMachine()->Reset_Trigger("ToIdle");
    pOwner->Push_Invincible();

    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Dash", CJaneDoeState_Dash::Create());
        m_pSubStateMachine->Register_State("Backstep", CJaneDoeState_BackStep::Create());

        m_pSubStateMachine->Get_State("Dash")->Set_Tag("Dash");
        m_pSubStateMachine->Get_State("Backstep")->Set_Tag("Backstep");
    }

    if (pOwner->Is_Passion())
    {
        m_iMask = pOwner->Get_CCT()->Get_CollisionMask();
        pOwner->Get_CCT()->Set_CollisionMask(m_iMask - ENUM(COLLISION_GROUP::MONSTER));
        pOwner->Set_LookTarget(false);
        m_pSubStateMachine->Set_Bool("Penetrate", true);
    }

    if (pOwner->Is_Move())
        m_pSubStateMachine->Set_DefaultState("Dash");
    else
        m_pSubStateMachine->Set_DefaultState("Backstep");

    m_pSubStateMachine->Reset_Trigger("Complete");
    m_pSubStateMachine->Set_Bool("Extreme", false);
    m_pSubStateMachine->Set_Int("ExitMode", 0);

    __super::Enter(pOwner);
}

void CJaneDoeState_Evade::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_fAnimProgress < 0.12f && m_fAnimProgress >= 0.02f)
    {  
        if (pOwner->Is_Perfect() && !m_pSubStateMachine->Get_Bool("Extreme"))
        {
            BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::EVADE);
            m_pSubStateMachine->Set_Bool("Extreme", true);
        }
    }

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        _int iExitMode = m_pSubStateMachine->Get_Int("ExitMode");
        m_pSubStateMachine->Reset_Trigger("Complete");
        CStateMachine<CJaneDoe>* pRootFSM = pOwner->Get_StateMachine();

        switch (iExitMode)
        {
        case 5: // CounterAttack
            pRootFSM->Set_Int("AttackEntryMode", 5);
            pRootFSM->Set_Trigger("Attack");
            break;
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

void CJaneDoeState_Evade::Exit(CJaneDoe* pOwner)
{
    if (pOwner->Is_Passion() && m_pOwnerStateMachine->Get_Bool("Penetrate"))
    {
        pOwner->Get_CCT()->Set_CollisionMask(m_iMask);
    }
    pOwner->Set_LookTarget(true);
    pOwner->Pop_Invincible();
    pOwner->Set_InvincibleTimer(0.5f); // 추가 무적 설정
    __super::Exit(pOwner);
}