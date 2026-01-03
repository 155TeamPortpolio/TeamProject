#include "pch.h"
#include "GameInstance.h"
#include "CorinState_Attack.h"
#include "CorinState_NormalAttack.h"
#include "CorinState_RushAttack.h"
#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Attack::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("NormalAttack", CCorinState_NormalAttack::Create());
        m_pSubStateMachine->Register_State("RushAttack", CCorinState_RushAttack::Create());

        m_pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");
        m_pSubStateMachine->Get_State("RushAttack")->Set_Tag("RushAttack");

        m_pSubStateMachine->Set_DefaultState("NormalAttack");
    }

    pOwner->Get_StateMachine()->Set_Bool("AttackEnd", false);   // ÃÊ±âÈ­

    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("AttackEntryMode");
    pOwner->Get_StateMachine()->Set_Int("AttackEntryMode", 0);

    switch (iEntryMode)
    {
    case 1:
        m_pSubStateMachine->Set_DefaultState("RushAttack");
        break;
    default:
        m_pSubStateMachine->Set_DefaultState("NormalAttack");
        break;
    }
    __super::Enter(pOwner);
}

void CCorinState_Attack::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    if (!m_pSubStateMachine) return;
    IHState<CCorin>* pAttackType = dynamic_cast<IHState<CCorin>*>(
        m_pSubStateMachine->Get_CurrentState());

    if (pAttackType && pAttackType->Is_EndState())
    {
        IBaseState<CCorin>* pEnd = pAttackType->Get_SubStateMachine()->Get_CurrentState();
        if (pEnd && (pOwner->Is_Input() || pEnd->Is_AnimEnd()))
        {
            pOwner->Get_StateMachine()->Set_Trigger("ToIdle");
        }
    }
}

//_bool CCorinState_Attack::Handle_Transition(CCorin* pOwner, const string& strState)
//{
//    if (strState == "Evade")
//        return true;
//
//    if (strState != "Attack")
//    {
//        if (!m_pSubStateMachine) return true;
//        IHState<CCorin>* pAttackType = dynamic_cast<IHState<CCorin>*>(
//            m_pSubStateMachine->Get_CurrentState()
//            );
//
//        if (!pAttackType) return true;
//        if (!pAttackType->Is_EndState()) return false;
//        if (strState == "Idle")
//        {
//            IBaseState<CCorin>* pEnd = pAttackType->Get_SubStateMachine()->Get_CurrentState();
//            if (pEnd && (pEnd->Is_AnimEnd() || pOwner->Is_Input())) return true;
//        }
//        return false;
//    }
//    return true;
//}