#include "pch.h"
#include "JaneDoeState_Attack.h"
#include "GameInstance.h"
#include "JaneDoeState_NormalAttack.h"
#include "JaneDoeState_RushAttack.h"
#include "JaneDoeState_ExAttack.h"
#include "JaneDoeState_UltimateAttack.h"
#include "JaneDoeState_BranchAttack.h"
#include "JaneDoeState_CounterAttack.h"
#include "JaneDoeState_AssaultAttack.h"
#include "JaneDoe.h"

#include "CharacterController.h"
#include "BattleSystem.h"

CJaneDoeState_Attack* CJaneDoeState_Attack::Create()
{
    auto pInstance = new CJaneDoeState_Attack();
    pInstance->m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("NormalAttack", CJaneDoeState_NormalAttack::Create());
    pSubStateMachine->Register_State("RushAttack", CJaneDoeState_RushAttack::Create());
    pSubStateMachine->Register_State("ExAttack", CJaneDoeState_ExAttack::Create());
    pSubStateMachine->Register_State("UltimateAttack", CJaneDoeState_UltimateAttack::Create());
    pSubStateMachine->Register_State("BranchAttack", CJaneDoeState_BranchAttack::Create());
    pSubStateMachine->Register_State("CounterAttack", CJaneDoeState_CounterAttack::Create());
    pSubStateMachine->Register_State("AssaultAttack", CJaneDoeState_AssaultAttack::Create());

    pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");
    pSubStateMachine->Get_State("RushAttack")->Set_Tag("RushAttack");
    pSubStateMachine->Get_State("ExAttack")->Set_Tag("ExAttack");
    pSubStateMachine->Get_State("UltimateAttack")->Set_Tag("UltimateAttack");
    pSubStateMachine->Get_State("BranchAttack")->Set_Tag("BranchAttack");
    pSubStateMachine->Get_State("CounterAttack")->Set_Tag("CounterAttack");
    pSubStateMachine->Get_State("AssaultAttack")->Set_Tag("AssaultAttack");

    pSubStateMachine->Register_Transition("NormalAttack", "BranchAttack",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "Salchow");

    pSubStateMachine->Register_Transition("NormalAttack", "ExAttack",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToExAttack");

    pSubStateMachine->Register_AnyStateTransition("UltimateAttack",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToUltimate");

    pSubStateMachine->Set_DefaultState("NormalAttack");

    return pInstance;
}

void CJaneDoeState_Attack::Enter(CJaneDoe* pOwner)
{
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("AttackEntryMode");
    pOwner->Get_StateMachine()->Set_Int("AttackEntryMode", 0);
    m_pSubStateMachine->Reset_Trigger("Salchow");

    switch (iEntryMode)
    {
    case 1:
        m_pSubStateMachine->Set_DefaultState("RushAttack");
        break;
    case 2:
        m_pSubStateMachine->Set_DefaultState("ExAttack");
        break;
    case 3:
        m_pSubStateMachine->Set_DefaultState("UltimateAttack");
        break;
    case 4:
        m_pSubStateMachine->Set_DefaultState("BranchAttack");
        break;
    case 5:
        m_pSubStateMachine->Set_DefaultState("CounterAttack");
        break;
    case 6:
        m_pSubStateMachine->Set_DefaultState("AssaultAttack");
        break;
    default:
        m_pSubStateMachine->Set_DefaultState("NormalAttack");
        break;
    }

    m_fHoldTime = 0.f;

    __super::Enter(pOwner);
}

void CJaneDoeState_Attack::Update(CJaneDoe* pOwner, _float dt)
{
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("AttackEntryMode");
    if (iEntryMode == 3)
    {
        pOwner->Get_StateMachine()->Set_Int("AttackEntryMode", 0);
        m_pSubStateMachine->Set_Trigger("ToUltimate");
    }

    if (pOwner->Can_Salchow())
    {
        if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Hold(MOUSE_BTN::LB))
        {
            m_fHoldTime += dt;
            if (m_fHoldTime >= 0.3f)
            {
                m_pSubStateMachine->Set_Trigger("Salchow");
                m_fHoldTime = 0.f;
                pOwner->Set_Salchow(false);
            }
        }
        else
        {
            m_fHoldTime = 0.f;
        }
    }

    if (pOwner->Is_LookTarget())
        pOwner->Look_Target();

    __super::Update(pOwner, dt);
}

void CJaneDoeState_Attack::Exit(CJaneDoe* pOwner)
{
    pOwner->End_AllAttackColliders();
    pOwner->Unlock_Move();
    __super::Exit(pOwner);
}

