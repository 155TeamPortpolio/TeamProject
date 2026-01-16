#include "pch.h"
#include "JaneDoeState_UltimateAttack.h"
#include "JaneDoe.h"

#include "CamDirector.h"

void CJaneDoeState_UltimateAttack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();

        m_pSubStateMachine->Register_State("Start", CJaneDoeState_UltimateAttack_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CJaneDoeState_UltimateAttack_Loop::Create());
        m_pSubStateMachine->Register_State("End", CJaneDoeState_UltimateAttack_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);

    CCamDirector::GetInstance()->RequestSequence("Ultimate/Jane_Ultimate");
}

void CJaneDoeState_UltimateAttack::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CJaneDoeState_UltimateAttack_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_Start")
        //.Speed(2.f)
        .Apply();
}

void CJaneDoeState_UltimateAttack_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_UltimateAttack_Loop::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex")
        //.Speed(2.f)
        .Apply();
}

void CJaneDoeState_UltimateAttack_Loop::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_UltimateAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_End")
        //.Speed(2.f)
        .Apply();
}

void CJaneDoeState_UltimateAttack_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}
