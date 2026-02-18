#include "pch.h"
#include "JaneDoeState_Dash.h"
#include "JaneDoe.h"

#include "JaneDoeState_Evade.h"
#include "CharacterController.h"

#include "GameInstance.h"
#include "EffectContainer.h"


void CJaneDoeState_Dash::Enter(CJaneDoe* pOwner)
{
    pOwner->Use_Evade();
    if (pOwner->Is_Passion())
    {
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Front_03")
            .Speed(1.f)
            .Apply();
    }
    else
    {
        m_bEvadeType = !m_bEvadeType;

        string strEvade = m_bEvadeType ? "Evade_Front_01" : "Evade_Front_02";
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + strEvade)
            .Speed(1.f)
            .Apply();
    }

    _vector3 vDir = pOwner->Get_InputDir();
    if (vDir.Length() > 0.01f)
        pOwner->Rotate(vDir);

}

void CJaneDoeState_Dash::Update(CJaneDoe* pOwner, _float dt)
{
    IHState<CJaneDoe>* pEvade = Get_ParentState();
    if (!pEvade || !pEvade->Get_SubStateMachine()) return;
    auto pSubMachine = pEvade->Get_SubStateMachine();

    //pOwner->Process_RootMotion(dt);
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    if (pOwner->Is_Passion())
    {
        if (IsCrossAnimProgress(0.02f))
        {
            pOwner->SetRenderLayer(RENDER_LAYER::CustomOnly);
        }
        for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
        {
            if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
            if (Event.Tag == "MotionBlur")
            {
                pOwner->Add_MotionBlur();
            }
        }
        if (IsCrossAnimProgress(0.08f))
        {
            pOwner->SetRenderLayer(RENDER_LAYER::Default);
            pOwner->Clear_MotionBlur();
        }
    }

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

void CJaneDoeState_Dash::Exit(CJaneDoe* pOwner)
{
    pOwner->SetRenderLayer(RENDER_LAYER::Default);
    pOwner->Clear_MotionBlur();
}

void CJaneDoeState_Dash::Update_Effects(CJaneDoe* pOwner)
{
    if (IsCrossAnimProgress(0.02f))
    {
        pOwner->Play_Effect("Player_Run_Start0", _vector3(0.f, 1.1f, 1.3f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
        pOwner->Play_Effect("Player_Run_Start1", _vector3(0.f, 0.15f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
    }
}
