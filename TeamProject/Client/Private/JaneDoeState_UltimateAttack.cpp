#include "pch.h"
#include "JaneDoeState_UltimateAttack.h"

#include "BattleSystem.h"

#include "JaneDoe.h"

#include "CamDirector.h"

void CJaneDoeState_UltimateAttack::Enter(CJaneDoe* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Lock_Move();

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

void CJaneDoeState_UltimateAttack::Exit(CJaneDoe* pOwner)
{
    __super::Exit(pOwner);
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

    m_fDamageTimer = m_fDamageInterval;
}

void CJaneDoeState_UltimateAttack_Loop::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "UltimateEnd")
        {
            m_bDamageActive = false;
        }
    }

    if (m_bDamageActive)
    {
        m_fDamageTimer += dt;
        if (m_fDamageTimer >= m_fDamageInterval)
        {
            m_fDamageTimer -= m_fDamageInterval;
            BattleSystem()->TakeAllDamage({ HIT_TYPE::ONCE, DAMAGE_TYPE::ULTIMATE, 10.f, 0, 0 });
        }
    }
}

void CJaneDoeState_UltimateAttack_Loop::Exit(CJaneDoe* pOwner)
{
    m_fDamageTimer = 0.f;
    m_bDamageActive = true;
}

void CJaneDoeState_UltimateAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_End")
        //.Speed(2.f)
        .Apply();

    pOwner->Pop_Invincible();
    pOwner->Unlock_Move();
}

void CJaneDoeState_UltimateAttack_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}
