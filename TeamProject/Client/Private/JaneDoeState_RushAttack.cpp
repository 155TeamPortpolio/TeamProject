#include "pch.h"
#include "JaneDoeState_RushAttack.h"
#include "JaneDoe.h"

void CJaneDoeState_RushAttack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();

        m_pSubStateMachine->Register_State("Rush03_Start", CJaneDoeState_Rush03_Start::Create());
        m_pSubStateMachine->Register_State("Rush03_End", CJaneDoeState_Rush03_End::Create());
        m_pSubStateMachine->Register_State("Rush02_Start", CJaneDoeState_Rush02_Start::Create());
        m_pSubStateMachine->Register_State("Rush02_End", CJaneDoeState_Rush02_End::Create());
        m_pSubStateMachine->Register_State("Rush01_Start", CJaneDoeState_Rush01_Start::Create());
        m_pSubStateMachine->Register_State("Rush01_End", CJaneDoeState_Rush01_End::Create());

        m_pSubStateMachine->Register_Transition("Rush01_Start", "Rush01_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Rush02_Start", "Rush02_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Rush03_Start", "Rush03_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Rush03_Start");
    }

    if (pOwner->IsPassion())
    {
        m_pSubStateMachine->Set_DefaultState("Rush03_Start");
        m_pSubStateMachine->Get_State("Rush03_End")->Set_Tag("End");
    }
    else if (pOwner->Get_EvadeCount() <= 1)
    {
        m_pSubStateMachine->Set_DefaultState("Rush02_Start");
        m_pSubStateMachine->Get_State("Rush02_End")->Set_Tag("End");
    }
    else
    {
        m_pSubStateMachine->Set_DefaultState("Rush01_Start");
        m_pSubStateMachine->Get_State("Rush01_End")->Set_Tag("End");
    }

    __super::Enter(pOwner);
}

void CJaneDoeState_RushAttack::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CJaneDoeState_Rush01_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_01")
        .Apply();
}

void CJaneDoeState_Rush01_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Rush01_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_01_End")
        .Apply();
}

void CJaneDoeState_Rush02_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_02")
        .Apply();
}

void CJaneDoeState_Rush02_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Rush02_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_02_End")
        .Apply();
}

void CJaneDoeState_Rush03_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_03")
        .Apply();
}

void CJaneDoeState_Rush03_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Rush03_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_03_End")
        .Apply();
}
