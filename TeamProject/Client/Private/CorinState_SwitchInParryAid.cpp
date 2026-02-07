#include "pch.h"
#include "CorinState_SwitchInParryAid.h"

#include "GameInstance.h"
#include "BattleSystem.h"

#include "Corin.h"
#include "Enemy.h"

#include "CharacterController.h"

CCorinState_SwitchInParryAid* CCorinState_SwitchInParryAid::Create()
{
    auto pInstance = new CCorinState_SwitchInParryAid();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("ParryAid_Start", CCorinState_SwitchInParryAid_Start::Create());
    pSubStateMachine->Register_State("L_Loop", CCorinState_SwitchInParryAid_L_Loop::Create());
    pSubStateMachine->Register_State("L_End", CCorinState_SwitchInParryAid_L_End::Create());
    pSubStateMachine->Register_State("H_Loop", CCorinState_SwitchInParryAid_H_Loop::Create());
    pSubStateMachine->Register_State("H_End", CCorinState_SwitchInParryAid_H_End::Create());

    pSubStateMachine->Get_State("L_End")->Set_Tag("End");
    pSubStateMachine->Get_State("H_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("ParryAid_Start", "L_Loop",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("L_Loop", "L_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("ParryAid_Start");

    pSubStateMachine->Set_Bool("ReserveAssaultAid", false);
    return pInstance;
}

void CCorinState_SwitchInParryAid::Enter(CCorin* pOwner)
{
    pOwner->Lock_Move();
    pOwner->Lock_Rotate();

    OBJECT_HANDLE handle = pOwner->Get_ParryHandle();
    if (handle.isValid())
    {
        dynamic_cast<CEnemy*>(handle.Get())->Parried();
        BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::PARRY);
        TARGET_LOCK_DESC desc;
        desc.bLock = true;
        desc.tHandle = handle;
        EventSystem()->Broadcast<TARGET_LOCK_DESC>({ desc });
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
        .BlendDuration(0.1f)
        .ReserveSpeed(0.f, 1.f, 2.f, EaseType::OutQuint)
        .EndAt(0.4f)
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
        .BlendDuration(0.1f)
        .Speed(2.f)
        .Apply();

    OBJECT_HANDLE handle = pOwner->Get_ParryHandle();
    if (handle.isValid())
    {
        TARGET_LOCK_DESC desc;
        desc.bLock = false;
        desc.tHandle = handle;
        EventSystem()->Broadcast<TARGET_LOCK_DESC>({desc});
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
    if (m_pOwnerStateMachine->Get_Bool("ReserveAssaultAid"))
    {
        pOwner->Get_StateMachine()->Set_Trigger("Attack");
        pOwner->Get_StateMachine()->Set_Int("AttackEntryMode", 6);
    }

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
