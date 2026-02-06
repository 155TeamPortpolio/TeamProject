#include "pch.h"
#include "MiyabiState_SwitchInParryAid.h"

#include "GameInstance.h"
#include "BattleSystem.h"

#include "Miyabi.h"
#include "Enemy.h"

CMiyabiState_SwitchInParryAid* CMiyabiState_SwitchInParryAid::Create()
{
    auto pInstance = new CMiyabiState_SwitchInParryAid();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("ParryAid_Start", CMiyabiState_SwitchInParryAid_Start::Create());
    pSubStateMachine->Register_State("L_Loop", CMiyabiState_SwitchInParryAid_L_Loop::Create());
    pSubStateMachine->Register_State("L_End", CMiyabiState_SwitchInParryAid_L_End::Create());
    pSubStateMachine->Register_State("H_Loop", CMiyabiState_SwitchInParryAid_H_Loop::Create());
    pSubStateMachine->Register_State("H_End", CMiyabiState_SwitchInParryAid_H_End::Create());

    pSubStateMachine->Get_State("L_End")->Set_Tag("End");
    pSubStateMachine->Get_State("H_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("ParryAid_Start", "L_Loop",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_GREATER, "", 0.4f);
    pSubStateMachine->Register_Transition("L_Loop", "L_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("ParryAid_Start");

    pSubStateMachine->Set_Bool("ReserveAssaultAid", false);
    return pInstance;
}

void CMiyabiState_SwitchInParryAid::Enter(CMiyabi* pOwner)
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

void CMiyabiState_SwitchInParryAid::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        IHState<CMiyabi>* pSwitchIn = Get_ParentState();
        if (pSwitchIn && pSwitchIn->Get_SubStateMachine())
        {
            pSwitchIn->Get_SubStateMachine()->Set_Int("ExitMode", 0);  // Idle·Î
            pSwitchIn->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }
}

void CMiyabiState_SwitchInParryAid::Exit(CMiyabi* pOwner)
{
    pOwner->Unlock_Move();
    pOwner->Unlock_Rotate();

    OBJECT_HANDLE handle = pOwner->Get_ParryHandle();
    if (handle.isValid())
    {
        TARGET_LOCK_DESC desc;
        desc.bLock = false;
        desc.tHandle = handle;
        EventSystem()->Broadcast<TARGET_LOCK_DESC>({ desc });
    }

    __super::Exit(pOwner);
}

void CMiyabiState_SwitchInParryAid_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_Start")
        .Loop(false)
        .BlendDuration(0.1f)
        .Speed(2.5f)
        .Apply();
}

void CMiyabiState_SwitchInParryAid_Start::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_SwitchInParryAid_L_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_L")
        .Loop(false)
        .BlendDuration(0.1f)
        .Speed(2.f)
        .Apply();

    OBJECT_HANDLE handle = pOwner->Get_ParryHandle();
    if (handle.isValid())
    {
        dynamic_cast<CEnemy*>(handle.Get())->Parried();
        BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::PARRY);
    }
}

void CMiyabiState_SwitchInParryAid_L_Loop::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_SwitchInParryAid_L_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_L_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CMiyabiState_SwitchInParryAid_L_End::Update(CMiyabi* pOwner, _float dt)
{
    if (m_pOwnerStateMachine->Get_Bool("ReserveAssaultAid"))
    {
        pOwner->Get_StateMachine()->Set_Trigger("Attack");
        pOwner->Get_StateMachine()->Set_Int("AttackEntryMode", 6);
    }

    pOwner->Process_RootMotion(dt);

    IHState<CMiyabi>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}

void CMiyabiState_SwitchInParryAid_H_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_H")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CMiyabiState_SwitchInParryAid_H_Loop::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_SwitchInParryAid_H_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_H_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CMiyabiState_SwitchInParryAid_H_End::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    IHState<CMiyabi>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
