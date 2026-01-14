#include "pch.h"
#include "JaneDoeState_Attack.h"
#include "GameInstance.h"
#include "JaneDoeState_NormalAttack.h"
#include "JaneDoeState_RushAttack.h"
#include "JaneDoeState_ExAttack.h"
#include "JaneDoeState_UltimateAttack.h"
#include "JaneDoeState_BranchAttack.h"
#include "JaneDoe.h"

#include "CharacterController.h"
#include "BattleSystem.h"

void CJaneDoeState_Attack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("NormalAttack", CJaneDoeState_NormalAttack::Create());
        m_pSubStateMachine->Register_State("RushAttack", CJaneDoeState_RushAttack::Create());
        m_pSubStateMachine->Register_State("ExAttack", CJaneDoeState_ExAttack::Create());
        m_pSubStateMachine->Register_State("UltimateAttack", CJaneDoeState_UltimateAttack::Create());
        m_pSubStateMachine->Register_State("BranchAttack", CJaneDoeState_BranchAttack::Create());

        m_pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");
        m_pSubStateMachine->Get_State("RushAttack")->Set_Tag("RushAttack");
        m_pSubStateMachine->Get_State("ExAttack")->Set_Tag("ExAttack");
        m_pSubStateMachine->Get_State("UltimateAttack")->Set_Tag("UltimateAttack");
        m_pSubStateMachine->Get_State("BranchAttack")->Set_Tag("BranchAttack");

        m_pSubStateMachine->Register_Transition("NormalAttack", "BranchAttack",
            CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "Passion");

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
    case 4:
        m_pSubStateMachine->Set_DefaultState("BranchAttack");
        break;
    default:
        m_pSubStateMachine->Set_DefaultState("NormalAttack");
        break;
    }

    m_fHoldTime = 0.f;

    __super::Enter(pOwner);
}

void CJaneDoeState_Attack::Update(CJaneDoe * pOwner, _float dt)
{
    if (true/*pOwner->Has_PassionSkill()*/)
    {
        if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Hold(MOUSE_BTN::LB))
        {
            m_fHoldTime += dt;
            if (m_fHoldTime >= 0.3f)
            {
                m_pSubStateMachine->Set_Trigger("Passion");
                m_fHoldTime = 0.f;
                pOwner->Set_PassionSkill(false);
            }
        }
        else
        {
            m_fHoldTime = 0.f;
        }
    }

    if (pOwner->Get_TargetHandle().isValid())
    {
        auto target = pOwner->Get_TargetHandle().Get();
        _vector3 vLook = target->Get_WorldPos() - pOwner->Get_WorldPos();
        vLook.y = 0;
        vLook.Normalize();
        pOwner->Get_Component<CTransform>()->Set_Look(vLook);
    }

    __super::Update(pOwner, dt);
}

