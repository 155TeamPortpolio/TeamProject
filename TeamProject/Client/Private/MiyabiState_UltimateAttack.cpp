#include "pch.h"
#include "MiyabiState_UltimateAttack.h"

#include "BattleSystem.h"

#include "Miyabi.h"

#include "CamDirector.h"

CMiyabiState_UltimateAttack* CMiyabiState_UltimateAttack::Create()
{
    auto pInstance = new CMiyabiState_UltimateAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("UltimateAttack_Start", CMiyabiState_UltimateAttack_Start::Create());
    pSubStateMachine->Register_State("UltimateAttack_Loop", CMiyabiState_UltimateAttack_Loop::Create());
    pSubStateMachine->Register_State("UltimateAttack_End", CMiyabiState_UltimateAttack_End::Create());

    pSubStateMachine->Get_State("UltimateAttack_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("UltimateAttack_Start", "UltimateAttack_Loop",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("UltimateAttack_Loop", "UltimateAttack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("UltimateAttack_Start");

    return pInstance;
}

void CMiyabiState_UltimateAttack::Enter(CMiyabi* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Lock_Move();
    pOwner->Increase_Frost(3);

    __super::Enter(pOwner);

    //CCamDirector::GetInstance()->RequestSequence(CamSeqType::Ultimate);
}

void CMiyabiState_UltimateAttack::Update(CMiyabi* pOwner, _float dt)
{
    auto pMiyabiState = pOwner->Get_StateMachine();
    if (pMiyabiState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" &&
            Is_AnimEnd())
        {
            pMiyabiState->Set_Trigger("SwitchOut");
            pMiyabiState->Set_Bool("OutReserve", false);
        }
    }
    __super::Update(pOwner, dt);
}

void CMiyabiState_UltimateAttack::Exit(CMiyabi* pOwner)
{
    pOwner->Pop_Invincible();
    __super::Exit(pOwner);
}

void CMiyabiState_UltimateAttack_Start::Enter(CMiyabi* pOwner)
{
    BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::ULTIMATE);
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_Start")
        .Apply();
}

void CMiyabiState_UltimateAttack_Start::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_UltimateAttack_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex")
        .Apply();

    m_bDamageActive = false;
}

void CMiyabiState_UltimateAttack_Loop::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

        if (Event.Tag == "UltimateAttack")
        {
            BattleSystem()->TakeAllDamage(HitDesc()
                .Name(pOwner->Get_CharacterName())
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 1.567f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::ULTIMATE)
            );
            BattleSystem()->TakeAllDamage(HitDesc()
                .Name(pOwner->Get_CharacterName())
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 1.567f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::ULTIMATE)
            );
        }

        if (Event.Tag == "UltimateStart")
        {
            m_bDamageActive = true;
            m_fDamageTimer = m_fDamageInterval;
        }
        else if (Event.Tag == "UltimateEnd")
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
                .Name(pOwner->Get_CharacterName())
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 3.135f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::ULTIMATE)
            );
        }
    }

    Update_Effects(pOwner);
}

void CMiyabiState_UltimateAttack_Loop::Exit(CMiyabi* pOwner)
{
    m_fDamageTimer = 0.f;
    m_bDamageActive = false;
}

void CMiyabiState_UltimateAttack_Loop::Update_Effects(CMiyabi* pOwner)
{
    // 1
    if (IsCrossAnimProgress(0.06f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash0", _vector3(0.f, 0.7f, 13.6f), _quaternion(-0.27f, 0.91f, 0.11f, -0.31f), false);
        pOwner->Play_Effect("Miyabi_Ultimate0_Sting0", _vector3(0.f, 0.2f, 0.f), _quaternion(0.f, -0.71f, 0.f, 0.71f), false);
    }
    if (IsCrossAnimProgress(0.08f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash1", _vector3(0.f, 0.7f, 13.6f), _quaternion(-0.37f, -0.02f, -0.22f, 0.9f), false);
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash2", _vector3(0.f, 0.7f, 13.6f), _quaternion(0.14f, -0.5f, 0.84f, 0.17f), false);
    }

    // 2
    if(IsCrossAnimProgress(0.2f))
        pOwner->Play_Effect("Miyabi_Ultimate1_Sting0", _vector3(1.1f, 0.2f, 12.9f), _quaternion(0.f, 0.49f, 0.f, 0.87f), false);
    if (IsCrossAnimProgress(0.21f))
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash3", _vector3(3.f, 0.7f, 8.6f), _quaternion(-0.44f, 0.73f, 0.25f, -0.46f), false);
    if (IsCrossAnimProgress(0.23f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash4", _vector3(3.f, 0.7f, 8.6f), _quaternion(0.71f, -0.09f, 0.12f, -0.69f), false);
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash5", _vector3(3.f, 0.7f, 8.6f), _quaternion(-0.4f, 0.6f, -0.65f, -0.25f), false);
    }
    // 3
    if(IsCrossAnimProgress(0.24f))
        pOwner->Play_Effect("Miyabi_Ultimate1_Sting1", _vector3(0.2f, 0.2f, 9.f), _quaternion(0.f, 0.95f, 0.f, 0.3f), false);
    if (IsCrossAnimProgress(0.25f))
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash0", _vector3(-4.f, 0.7f, 5.f), _quaternion(-0.44f, 0.73f, 0.25f, -0.46f), false);
    if (IsCrossAnimProgress(0.27f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash1", _vector3(-4.f, 0.7f, 5.f), _quaternion(0.71f, -0.09f, 0.12f, -0.69f), false);
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash2", _vector3(-4.f, 0.7f, 5.f), _quaternion(-0.4f, 0.6f, -0.65f, -0.25f), false);
    }
    // 4
    if(IsCrossAnimProgress(0.28f))
        pOwner->Play_Effect("Miyabi_Ultimate1_Sting2", _vector3(-4.1f, 0.2f, 3.6f), _quaternion(0.f, 0.36f, 0.f, 0.93f), false);
    if (IsCrossAnimProgress(0.29f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash3", _vector3(0.f, 0.7f, 0.f), _quaternion(-0.27f, 0.67f, 0.69f, -0.08f), false);
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash4", _vector3(0.f, 0.7f, 0.f), _quaternion(-0.35f, 0.87f, 0.24f, 0.26f), false);

    }
    if (IsCrossAnimProgress(0.31f))
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash5", _vector3(0.f, 0.7f, 0.f), _quaternion(-0.44f, -0.3f, -0.37f, 0.76f), false);

}

void CMiyabiState_UltimateAttack_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_End")
        .EndAt(0.3f)
        .Apply();

    pOwner->Unlock_Move();
}

void CMiyabiState_UltimateAttack_End::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}
