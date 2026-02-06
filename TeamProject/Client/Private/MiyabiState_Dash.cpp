#include "pch.h"
#include "MiyabiState_Dash.h"
#include "Miyabi.h"
#include "GameInstance.h"
#include "MiyabiState_Evade.h"
#include "CharacterController.h"

CMiyabiState_Dash* CMiyabiState_Dash::Create()
{
    auto pInstance = new CMiyabiState_Dash();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Dash_01", CMiyabiState_Dash_01::Create());
    pSubStateMachine->Register_State("Dash_02", CMiyabiState_Dash_02::Create());

    pSubStateMachine->Register_Transition("Dash_01", "Dash_02",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToDash02");

    pSubStateMachine->Set_DefaultState("Dash_01");

    return pInstance;
}

void CMiyabiState_Dash::Enter(CMiyabi* pOwner)
{
    m_pSubStateMachine->Reset_Trigger("ToDash02");

    pOwner->Use_Evade();
    _vector3 vDir = pOwner->Get_InputDir();
    if (vDir.Length() > 0.01f)
        pOwner->Rotate(vDir);

    __super::Enter(pOwner);
}

void CMiyabiState_Dash::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
}

void CMiyabiState_Dash::Exit(CMiyabi* pOwner)
{
    __super::Exit(pOwner);
}

void CMiyabiState_Dash_01::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Front")
        .Apply();
}

void CMiyabiState_Dash_01::Update(CMiyabi* pOwner, _float dt)
{
    auto pSubMachine = Get_ParentState()->Get_OwnerStateMachine();

    pOwner->Process_RootMotion(dt);

    if (pOwner->Is_Attack())
    {
        if (pSubMachine->Get_Bool("Extreme"))
        {
            // CounterAttack
            pSubMachine->Set_Int("ExitMode", 5);
            pSubMachine->Set_Trigger("Complete");
        }
        else
        {
            // RushAttack
            pSubMachine->Set_Int("ExitMode", 3);
            pSubMachine->Set_Trigger("Complete");
        }
        return;
    }

    if (IsCrossAnimProgress(0.08f) && InputDevice()->Mouse_Hold(MOUSE_BTN::RB))
    {
        m_pOwnerStateMachine->Set_Trigger("ToDash02");
    }

    if (m_fAnimProgress >= 0.1f)
    {
        if (pOwner->Can_Evade() && pOwner->Use_EvadeBuffer())
        {   // Idle -> Evade
            pSubMachine->Set_Int("ExitMode", 4);
            pSubMachine->Set_Trigger("Complete");
            return;
        }

        if (pOwner->Is_Move())
        {
            pSubMachine->Set_Int("ExitMode", 2);
            pSubMachine->Set_Trigger("Complete");
            return;
        }
    }

    if (m_fAnimProgress >= 0.7f)
    {   // Idle
        pSubMachine->Set_Int("ExitMode", 0);
        pSubMachine->Set_Trigger("Complete");
    }

    Update_Effects(pOwner);
}

void CMiyabiState_Dash_01::Update_Effects(CMiyabi* pOwner)
{
    if (IsCrossAnimProgress(0.02f))
    {
        pOwner->Play_Effect("Player_Run_Start0", _vector3(0.f, 1.1f, 1.3f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
        pOwner->Play_Effect("Player_Run_Start1", _vector3(0.f, 0.15f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
    }
}

void CMiyabiState_Dash_02::Enter(CMiyabi* pOwner)
{
    pOwner->Get_StateMachine()->Set_Bool("InDash02", true);
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Front_02")
        .Apply();
}

void CMiyabiState_Dash_02::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);
    auto pSubMachine = Get_ParentState()->Get_OwnerStateMachine();

    if (IsCrossAnimProgress(0.12f))
        pOwner->Get_StateMachine()->Set_Bool("InDash02", false);

    if (m_fAnimProgress >= 0.12f)
    {
        if (pOwner->Can_Evade() && pOwner->Use_EvadeBuffer())
        {   // Idle -> Evade
            pSubMachine->Set_Int("ExitMode", 4);
            pSubMachine->Set_Trigger("Complete");
            return;
        }
        if (pOwner->Is_Move())
        {
            pSubMachine->Set_Int("ExitMode", 2);
            pSubMachine->Set_Trigger("Complete");
            return;
        }
    }
    if (m_fAnimProgress >= 0.7f)
    {   // Idle
        pSubMachine->Set_Int("ExitMode", 0);
        pSubMachine->Set_Trigger("Complete");
    }

    Update_Effects(pOwner);
}

void CMiyabiState_Dash_02::Exit(CMiyabi* pOwner)
{
    pOwner->Get_StateMachine()->Set_Bool("InDash02", false);
}

void CMiyabiState_Dash_02::Update_Effects(CMiyabi* pOwner)
{
    if (IsCrossAnimProgress(0.02f))
    {
        pOwner->Play_Effect("Player_Run_Start0", _vector3(0.f, 1.1f, 1.3f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
        pOwner->Play_Effect("Player_Run_Start1", _vector3(0.f, 0.15f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
    }
}
