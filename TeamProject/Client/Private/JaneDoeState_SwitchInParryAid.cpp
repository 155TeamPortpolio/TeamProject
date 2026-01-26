#include "pch.h"
#include "JaneDoeState_SwitchInParryAid.h"

#include "JaneDoe.h"
#include "Enemy.h"

void CJaneDoeState_SwitchInParryAid::Enter(CJaneDoe* pOwner)
{
    pOwner->Lock_Move();
    pOwner->Lock_Rotate();
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        m_pSubStateMachine->Register_State("Start", CJaneDoeState_SwitchInParryAid_Start::Create());
        m_pSubStateMachine->Register_State("L_Loop", CJaneDoeState_SwitchInParryAid_L_Loop::Create());
        m_pSubStateMachine->Register_State("L_End", CJaneDoeState_SwitchInParryAid_L_End::Create());
        m_pSubStateMachine->Register_State("H_Loop", CJaneDoeState_SwitchInParryAid_H_Loop::Create());
        m_pSubStateMachine->Register_State("H_End", CJaneDoeState_SwitchInParryAid_H_End::Create());

        m_pSubStateMachine->Get_State("L_End")->Set_Tag("End");
        m_pSubStateMachine->Get_State("H_End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "L_Loop",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_GREATER, "", 0.2f);
        m_pSubStateMachine->Register_Transition("L_Loop", "L_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }

    __super::Enter(pOwner);
}

void CJaneDoeState_SwitchInParryAid::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        IHState<CJaneDoe>* pSwitchIn = Get_ParentState();
        if (pSwitchIn && pSwitchIn->Get_SubStateMachine())
        {
            pSwitchIn->Get_SubStateMachine()->Set_Int("ExitMode", 0);  // Idle·Î
            pSwitchIn->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }
}

void CJaneDoeState_SwitchInParryAid::Exit(CJaneDoe* pOwner)
{
    pOwner->Unlock_Move();
    pOwner->Unlock_Rotate();
    __super::Exit(pOwner);
}

void CJaneDoeState_SwitchInParryAid_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_Start")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInParryAid_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_SwitchInParryAid_L_Loop::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_L")
        .Loop(false)
        .Speed(1.2f)
        .Apply();

    if (pOwner->Get_ParryHandle().isValid())
    {
        CEnemy* pEnemy = dynamic_cast<CEnemy*>(pOwner->Get_ParryHandle().Get());
        if (pEnemy)
            pEnemy->Parried();
    }
}

void CJaneDoeState_SwitchInParryAid_L_Loop::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_SwitchInParryAid_L_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_L_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInParryAid_L_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    IHState<CJaneDoe>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}

void CJaneDoeState_SwitchInParryAid_H_Loop::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_H")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInParryAid_H_Loop::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_SwitchInParryAid_H_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_H_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInParryAid_H_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    IHState<CJaneDoe>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
