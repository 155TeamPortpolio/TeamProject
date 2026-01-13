#include "pch.h"
#include "JaneDoeState_RushAttack.h"
#include "JaneDoe.h"

void CJaneDoeState_RushAttack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();

        if (pOwner->IsPassion())
        {
            m_pSubStateMachine->Register_State("Rush_Start", CJaneDoeState_Rush03_Start::Create());
            m_pSubStateMachine->Register_State("Rush_End", CJaneDoeState_Rush03_End::Create());
        }
        else if (pOwner->Get_EvadeCount() <= 1)
        {
            m_pSubStateMachine->Register_State("Rush_Start", CJaneDoeState_Rush02_Start::Create());
            m_pSubStateMachine->Register_State("Rush_End", CJaneDoeState_Rush02_End::Create());
        }
        else
        {
            m_pSubStateMachine->Register_State("Rush_Start", CJaneDoeState_Rush01_Start::Create());
            m_pSubStateMachine->Register_State("Rush_End", CJaneDoeState_Rush01_End::Create());
        }

        m_pSubStateMachine->Get_State("Rush_End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Rush_Start", "Rush_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Rush_Start");
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
        //.Speed(2.f)
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
        //.Speed(2.f)
        .Apply();
}

void CJaneDoeState_Rush02_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_02")
        //.Speed(2.f)
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
        //.Speed(2.f)
        .Apply();
}

void CJaneDoeState_Rush03_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_03")
        //.Speed(2.f)
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
        //.Speed(2.f)
        .Apply();
}
