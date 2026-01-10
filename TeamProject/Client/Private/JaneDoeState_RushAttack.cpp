#include "pch.h"
#include "JaneDoeState_RushAttack.h"
#include "JaneDoe.h"

void CJaneDoeState_RushAttack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Rush_Start", CJaneDoeState_Rush_Start::Create());
        m_pSubStateMachine->Register_State("Rush_Explode", CJaneDoeState_Rush_Explode::Create());
        m_pSubStateMachine->Register_State("Rush_End", CJaneDoeState_Rush_End::Create());

        m_pSubStateMachine->Get_State("Rush_End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Rush_Start", "Rush_Explode",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Rush_Explode", "Rush_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Rush_Start");
    }
    __super::Enter(pOwner);
}

void CJaneDoeState_RushAttack::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CJaneDoeState_Rush_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_01")
        .Speed(2.f)
        .Apply();
}

void CJaneDoeState_Rush_Start::Update(CJaneDoe* pOwner, _float dt)
{
    CJaneDoe::ROOTMOTION_DESC desc;
    desc.iModeMask = ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION);
    desc.fMoveWeight = 0.5f;
    pOwner->Process_RootMotion(dt, desc);
}

void CJaneDoeState_Rush_Explode::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_Explode")
        .Speed(2.f)
        .Apply();
}

void CJaneDoeState_Rush_Explode::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Rush_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_End")
        .Speed(2.f)
        .Apply();
}
