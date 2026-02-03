#include "pch.h"
#include "MiyabiState_Attack.h"
#include "GameInstance.h"

#include "Miyabi.h"

#include "MiyabiState_NormalAttack.h"
//#include "MiyabiState_RushAttack.h"
//#include "MiyabiState_ExAttack.h"
//#include "MiyabiState_UltimateAttack.h"
//#include "MiyabiState_BranchAttack.h"
//#include "MiyabiState_CounterAttack.h"
//#include "MiyabiState_AssaultAttack.h"

#include "CharacterController.h"

CMiyabiState_Attack* CMiyabiState_Attack::Create()
{
    auto pInstance = new CMiyabiState_Attack();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("NormalAttack", CMiyabiState_NormalAttack::Create());
    //pSubStateMachine->Register_State("RushAttack", CMiyabiState_RushAttack::Create());
    //pSubStateMachine->Register_State("ExAttack", CMiyabiState_ExAttack::Create());
    //pSubStateMachine->Register_State("UltimateAttack", CMiyabiState_UltimateAttack::Create());
    //pSubStateMachine->Register_State("CounterAttack", CMiyabiState_CounterAttack::Create());
    //pSubStateMachine->Register_State("AssaultAttack", CMiyabiState_AssaultAttack::Create());

    pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");

    pSubStateMachine->Register_Transition("NormalAttack", "ExAttack",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToExAttack");

    pSubStateMachine->Register_AnyStateTransition("UltimateAttack",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToUltimate");

    pSubStateMachine->Set_DefaultState("NormalAttack");

    return pInstance;
}

void CMiyabiState_Attack::Enter(CMiyabi* pOwner)
{
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("AttackEntryMode");
    pOwner->Get_StateMachine()->Set_Int("AttackEntryMode", 0);

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
        m_pSubStateMachine->Set_DefaultState("ChargeAttack");
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

void CMiyabiState_Attack::Update(CMiyabi* pOwner, _float dt)
{
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("AttackEntryMode");
    if (iEntryMode == 3)
    {
        pOwner->Get_StateMachine()->Set_Int("AttackEntryMode", 0);
        m_pSubStateMachine->Set_Trigger("ToUltimate");
    }

    //if (pOwner->Has_Frost())
    //{
    //    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Hold(MOUSE_BTN::LB))
    //    {
    //        m_fHoldTime += dt;
    //        if (m_fHoldTime >= 0.3f)
    //        {
    //            m_pSubStateMachine->Set_Trigger("Salchow");
    //            m_fHoldTime = 0.f;
    //            pOwner->Set_Salchow(false);
    //        }
    //    }
    //    else
    //    {
    //        m_fHoldTime = 0.f;
    //    }
    //}

    if (pOwner->Is_LookTarget())
        pOwner->Look_Target();

    __super::Update(pOwner, dt);
}

void CMiyabiState_Attack::Exit(CMiyabi* pOwner)
{
    pOwner->End_AllAttackColliders();
    pOwner->Unlock_Move();
    __super::Exit(pOwner);
}