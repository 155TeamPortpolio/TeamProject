#include "pch.h"
#include "CorinState_Backstep.h"
#include "Corin.h"

#include "CharacterController.h"
#include "AudioSource.h"

void CCorinState_Backstep::Enter(CCorin* pOwner)
{
    pOwner->Use_Evade();
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Evade_Back")
        .Speed(1.f)
		.Apply();

    pOwner->Get_Component<CAudioSource>()->Slot("Corin_BackStep_01_SFX.wav").Attribute3D(true).Loop(false).Volume(0.35f).Play();

    pOwner->Stop_Effect("Corin_Saw_Slash0");
    pOwner->Stop_Effect("Corin_Ex_Saw_Slash0");
    pOwner->Stop_Effect("Corin_Ultimate_Saw_Slash0");
}

void CCorinState_Backstep::Update(CCorin* pOwner, _float dt)
{
    IHState<CCorin>* pEvade = Get_ParentState();
    if (!pEvade || !pEvade->Get_SubStateMachine()) return;
    auto pSubMachine = pEvade->Get_SubStateMachine();

    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

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

    if (m_fAnimProgress >= 0.25f)
    {
        if (pOwner->Can_Evade() && pOwner->Use_EvadeBuffer())
        {
            pSubMachine->Set_Int("ExitMode", 4);
            pSubMachine->Set_Trigger("Complete");
            return;
        }
    }

    if (m_fAnimProgress >= 0.5f)
    {   // Run
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
}
