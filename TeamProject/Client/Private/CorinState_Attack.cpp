#include "pch.h"
#include "GameInstance.h"
#include "CorinState_Attack.h"
#include "CorinState_NormalAttack.h"
#include "CorinState_RushAttack.h"
#include "CorinState_ExAttack.h"
#include "CorinState_UltimateAttack.h"
#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Attack::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("NormalAttack", CCorinState_NormalAttack::Create());
        m_pSubStateMachine->Register_State("RushAttack", CCorinState_RushAttack::Create());
        m_pSubStateMachine->Register_State("ExAttack", CCorinState_ExAttack::Create());
        m_pSubStateMachine->Register_State("UltimateAttack", CCorinState_UltimateAttack::Create());

        m_pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");
        m_pSubStateMachine->Get_State("RushAttack")->Set_Tag("RushAttack");
        m_pSubStateMachine->Get_State("ExAttack")->Set_Tag("ExAttack");
        m_pSubStateMachine->Get_State("UltimateAttack")->Set_Tag("UltimateAttack");

        m_pSubStateMachine->Register_Transition("NormalAttack", "ExAttack",
            CStateMachine<CCorin>::CONDITION_TRIGGER, "ToExAttack");

        m_pSubStateMachine->Set_DefaultState("NormalAttack");
    }

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
    default:
        m_pSubStateMachine->Set_DefaultState("NormalAttack");
        break;
    }
    __super::Enter(pOwner);
}

void CCorinState_Attack::Update(CCorin* pOwner, _float dt)
{
    if (pOwner->Get_TargetHandle().isValid())
    {
        auto target = pOwner->Get_TargetHandle().Get();
        _vector3 vLook = target->Get_WorldPos() - pOwner->Get_WorldPos();
        vLook.y = 0;
        vLook.Normalize();
        pOwner->Get_Component<CTransform>()->Set_Look(vLook);
        pOwner->Rotate(vLook);
    }
    __super::Update(pOwner, dt);
}

void CCorinState_Attack::Exit(CCorin* pOwner)
{
    pOwner->End_AllAttackColliders();
    pOwner->Unlock_Move();
    __super::Exit(pOwner);
}
