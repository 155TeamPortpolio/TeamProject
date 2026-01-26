#include "pch.h"
#include "CorinState_SwitchInParryAid.h"

#include "Corin.h"
#include "Enemy.h"

#include "CharacterController.h"

void CCorinState_SwitchInParryAid::Enter(CCorin* pOwner)
{
    pOwner->Lock_Move();
    pOwner->Lock_Rotate();
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Start", CCorinState_SwitchInParryAid_Start::Create());
        m_pSubStateMachine->Register_State("L_Loop", CCorinState_SwitchInParryAid_L_Loop::Create());
        m_pSubStateMachine->Register_State("L_End", CCorinState_SwitchInParryAid_L_End::Create());
        m_pSubStateMachine->Register_State("H_Loop", CCorinState_SwitchInParryAid_H_Loop::Create());
        m_pSubStateMachine->Register_State("H_End", CCorinState_SwitchInParryAid_H_End::Create());

        m_pSubStateMachine->Get_State("L_End")->Set_Tag("End");
        m_pSubStateMachine->Get_State("H_End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "L_Loop",
            CStateMachine<CCorin>::CONDITION_ANIMATION_GREATER, "", 0.4f);
        m_pSubStateMachine->Register_Transition("L_Loop", "L_End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }

    __super::Enter(pOwner);
}

void CCorinState_SwitchInParryAid::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        IHState<CCorin>* pSwitchIn = Get_ParentState();
        if (pSwitchIn && pSwitchIn->Get_SubStateMachine())
        {
            pSwitchIn->Get_SubStateMachine()->Set_Int("ExitMode", 0);  // Idle·Î
            pSwitchIn->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }
}

void CCorinState_SwitchInParryAid::Exit(CCorin* pOwner)
{
    pOwner->Unlock_Move();
    pOwner->Unlock_Rotate();
    __super::Exit(pOwner);
}

void CCorinState_SwitchInParryAid_Start::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_Start")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_SwitchInParryAid_Start::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_SwitchInParryAid_L_Loop::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_L")
        .Loop(false)
        .Speed(2.f)
        .Apply();

    OBJECT_HANDLE handle = pOwner->Get_ParryHandle();
    if (handle.isValid())
    {
        dynamic_cast<CEnemy*>(handle.Get())->Parried();
    }
}

void CCorinState_SwitchInParryAid_L_Loop::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_SwitchInParryAid_L_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_L_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_SwitchInParryAid_L_End::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    IHState<CCorin>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}

void CCorinState_SwitchInParryAid_H_Loop::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_H")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_SwitchInParryAid_H_Loop::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_SwitchInParryAid_H_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_H_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_SwitchInParryAid_H_End::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    IHState<CCorin>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
