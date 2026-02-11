#include "pch.h"
#include "CorinState_Idle.h"

#include "Corin.h"

#include "Animator3D.h"
#include "AudioSource.h"

void CCorinState_Idle::Enter(CCorin* pOwner)
{
    pOwner->Unlock_Move();
    _int iEntryMode = pOwner->Get_StateMachine()->Get_Int("IdleEntryMode");
    pOwner->Get_StateMachine()->Set_Int("IdleEntryMode", 0);

    if (iEntryMode == 1)
    {
        pOwner->Get_StateMachine()->Set_Trigger("Evade");
    }
    else if (iEntryMode == 2)
    {
        pOwner->Set_Switch(CCharacter::SWITCH::PARRYAID);
        pOwner->Get_StateMachine()->Set_Trigger("SwitchIn");
    }

    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Idle")
        .Loop(true)
        .Apply();

    m_idleVoiceAcc = 0.f;
    m_pIdleVoiceChannel = nullptr;

    __super::Enter(pOwner);

    pOwner->Stop_Effect("Corin_Saw_Slash0");
    pOwner->Stop_Effect("Corin_Ex_Saw_Slash0");
    pOwner->Stop_Effect("Corin_Ultimate_Saw_Slash0");
}

void CCorinState_Idle::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (!pOwner->Is_MainCharacter())
    {
        if (m_pIdleVoiceChannel)
            m_pIdleVoiceChannel->stop();

        m_pIdleVoiceChannel = nullptr;
        m_idleVoiceAcc = 0.f;
        return;
    }

    constexpr _float kIdleVoiceDelay = 3.f;

    bool isPlaying = false;
    if (m_pIdleVoiceChannel)
        m_pIdleVoiceChannel->isPlaying(&isPlaying);

    if (isPlaying)
    {
        m_idleVoiceAcc = 0.f;
        return;
    }

    m_idleVoiceAcc += dt;

    if (m_idleVoiceAcc < kIdleVoiceDelay)
        return;

    auto& sound = *pOwner->Get_Component<CAudioSource>();
    auto& slot = sound.Sequence("Idle_Voice").Attribute3D(true).Loop(0).PlayNext();

    m_pIdleVoiceChannel = slot.pChanel;
    m_idleVoiceAcc = 0.f;
}

void CCorinState_Idle::Exit(CCorin* pOwner)
{
    if (m_pIdleVoiceChannel)
        m_pIdleVoiceChannel->stop();

    m_idleVoiceAcc = 0.f;
    m_pIdleVoiceChannel = nullptr;

    __super::Exit(pOwner);
}
