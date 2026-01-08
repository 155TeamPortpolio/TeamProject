#include "pch.h"
#include "JaneDoeState_Attack.h"
#include "GameInstance.h"
#include "JaneDoeState_NormalAttack.h"
#include "JaneDoeState_RushAttack.h"
#include "JaneDoe.h"

#include "CharacterController.h"

void CJaneDoeState_Attack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("NormalAttack", CJaneDoeState_NormalAttack::Create());
        m_pSubStateMachine->Register_State("RushAttack", CJaneDoeState_RushAttack::Create());

        m_pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");
        m_pSubStateMachine->Get_State("RushAttack")->Set_Tag("RushAttack");

        m_pSubStateMachine->Set_DefaultState("NormalAttack");
    }

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

void CJaneDoeState_Attack::Update(CJaneDoe * pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

