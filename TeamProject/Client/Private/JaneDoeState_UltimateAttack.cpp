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

        m_pSubStateMachine->Register_State("UltimateAttack_Start", CJaneDoeState_UltimateAttack_Start::Create());
        m_pSubStateMachine->Register_State("UltimateAttack_Loop", CJaneDoeState_UltimateAttack_Loop::Create());
        m_pSubStateMachine->Register_State("UltimateAttack_End", CJaneDoeState_UltimateAttack_End::Create());

        m_pSubStateMachine->Get_State("UltimateAttack_End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("UltimateAttack_Start", "UltimateAttack_Loop",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("UltimateAttack_Loop", "UltimateAttack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("UltimateAttack_Start");
    }
    __super::Enter(pOwner);

    CCamDirector::GetInstance()->RequestSequence(CamSeqType::Ultimate);
}

void CJaneDoeState_UltimateAttack::Update(CJaneDoe* pOwner, _float dt)
{
    auto pJaneDoeState = pOwner->Get_StateMachine();
    if (pJaneDoeState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" &&
            Is_AnimEnd())
        {
            pJaneDoeState->Set_Trigger("SwitchOut");
            pJaneDoeState->Set_Bool("OutReserve", false);
        }
    }
    __super::Update(pOwner, dt);
}

void CJaneDoeState_UltimateAttack::Exit(CJaneDoe* pOwner)
{
    pOwner->Pop_Invincible();
    __super::Exit(pOwner);
}

void CJaneDoeState_UltimateAttack_Start::Enter(CJaneDoe* pOwner)
{
    //BattleSystem()->StartTimeScale(CBattleSystem::BATTLE_OBJ_TYPE::MONSTER, 2.4f, 0.f);
    BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::ULTIMATE);
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
            BattleSystem()->TakeAllDamage(HitDesc()
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 14.706f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::ULTIMATE)
            );
        }
    }

    Update_Effects(pOwner);
}

void CJaneDoeState_UltimateAttack_Loop::Exit(CJaneDoe* pOwner)
{
    m_fDamageTimer = 0.f;
    m_bDamageActive = true;
}

void CJaneDoeState_UltimateAttack_Loop::Update_Effects(CJaneDoe* pOwner)
{
    if (IsCrossAnimProgress(0.02f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash0_0", _vector3(-0.8f, 2.6f, 8.4f), _quaternion(-0.14f, 0.97f, 0.07f, 0.18f));
    if (IsCrossAnimProgress(0.04f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash1_0", _vector3(-0.2f, 2.2f, 7.5f), _quaternion(0.78f, -0.13f, -0.18f, -0.59f));
    if (IsCrossAnimProgress(0.1f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash0_1", _vector3(1.5f, 3.1f, 7.f), _quaternion(0.21f, 0.97f, 0.1f, 0.09f));
    if (IsCrossAnimProgress(0.14f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash3_0", _vector3(0.f, -0.3f, 3.2f), _quaternion(0.f, 1.f, 0.f, 0.f));
    if (IsCrossAnimProgress(0.24f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash0_2", _vector3(-0.8f, -0.5f, 3.2f), _quaternion(-0.67f, 0.09f, 0.12f, 0.73f));
    if (IsCrossAnimProgress(0.25f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash1_0", _vector3(-0.2f, 2.2f, 9.2f), _quaternion(0.69f, -0.46f, -0.34f, -0.45f));
    if (IsCrossAnimProgress(0.28f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash0_0", _vector3(0.f, 3.f, 7.f), _quaternion(-0.15f, 0.95f, 0.02f, 0.27f));
    if (IsCrossAnimProgress(0.3f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash4_0", _vector3(0.8f, 1.7f, 10.2f), _quaternion(-0.59f, 0.61f, 0.26f, 0.46f));
    if (IsCrossAnimProgress(0.35f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash0_1", _vector3(4.2f, 2.1f, 5.8f), _quaternion(0.97f, -0.22f, 0.05f, -0.09f));
    if (IsCrossAnimProgress(0.5f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash2_0", _vector3(1.1f, 4.2f, 3.f), _quaternion(0.6f, -0.57f, 0.42f, -0.38f));
}

void CJaneDoeState_UltimateAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_End")
        .EndAt(0.3f)
        .Apply();

    pOwner->Unlock_Move();
}

void CJaneDoeState_UltimateAttack_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}
