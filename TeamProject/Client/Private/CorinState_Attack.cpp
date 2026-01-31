#include "pch.h"
#include "GameInstance.h"
#include "CorinState_Attack.h"
#include "CorinState_NormalAttack.h"
#include "CorinState_RushAttack.h"
#include "CorinState_ExAttack.h"
#include "CorinState_UltimateAttack.h"
#include "CorinState_CounterAttack.h"
#include "CorinState_AssaultAttack.h"
#include "Corin.h"

#include "CharacterController.h"

CCorinState_Attack* CCorinState_Attack::Create()
{
    auto pInstance = new CCorinState_Attack();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("NormalAttack", CCorinState_NormalAttack::Create());
    pSubStateMachine->Register_State("RushAttack", CCorinState_RushAttack::Create());
    pSubStateMachine->Register_State("ExAttack", CCorinState_ExAttack::Create());
    pSubStateMachine->Register_State("UltimateAttack", CCorinState_UltimateAttack::Create());
    pSubStateMachine->Register_State("CounterAttack", CCorinState_CounterAttack::Create());
    pSubStateMachine->Register_State("AssaultAttack", CCorinState_AssaultAttack::Create());

    pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");
    pSubStateMachine->Get_State("RushAttack")->Set_Tag("RushAttack");
    pSubStateMachine->Get_State("ExAttack")->Set_Tag("ExAttack");
    pSubStateMachine->Get_State("UltimateAttack")->Set_Tag("UltimateAttack");
    pSubStateMachine->Get_State("CounterAttack")->Set_Tag("CounterAttack");
    pSubStateMachine->Get_State("AssaultAttack")->Set_Tag("AssaultAttack");

    pSubStateMachine->Register_Transition("NormalAttack", "ExAttack",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToExAttack");

    pSubStateMachine->Register_AnyStateTransition("UltimateAttack",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToUltimate");

    pSubStateMachine->Register_Transition("CounterAttack", "NormalAttack",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToNormalAttack");

    pSubStateMachine->Register_Transition("AssaultAttack", "NormalAttack",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToNormalAttack");

    pSubStateMachine->Set_DefaultState("NormalAttack");

    return pInstance;
}

void CCorinState_Attack::Enter(CCorin* pOwner)
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
    __super::Enter(pOwner);
}

void CCorinState_Attack::Update(CCorin* pOwner, _float dt)
{
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("AttackEntryMode");
    if (iEntryMode == 3)
    {
        pOwner->Get_StateMachine()->Set_Int("AttackEntryMode", 0);
        m_pSubStateMachine->Set_Trigger("ToUltimate");
    }

    pOwner->Look_Target();
    __super::Update(pOwner, dt);
}

void CCorinState_Attack::Exit(CCorin* pOwner)
{
    pOwner->End_AllAttackColliders();
    pOwner->Unlock_Move();
    __super::Exit(pOwner);
}
