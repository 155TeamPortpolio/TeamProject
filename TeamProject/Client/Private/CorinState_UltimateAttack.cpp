#include "pch.h"
#include "CorinState_UltimateAttack.h"

#include "BattleSystem.h"
#include "CamDirector.h"

#include "Corin.h"


void CCorinState_UltimateAttack::Enter(CCorin* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Lock_Move();

    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();

        m_pSubStateMachine->Register_State("Start", CCorinState_UltimateAttack_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CCorinState_UltimateAttack_Loop::Create());
        m_pSubStateMachine->Register_State("End", CCorinState_UltimateAttack_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);

    CamDirector()->RequestSequence(CamSeqType::Ultimate);
}

void CCorinState_UltimateAttack::Update(CCorin* pOwner, _float dt)
{
    for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "SawStart")
        {
            pOwner->Begin_AttackCollider("Saw",
                HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(Helper::Get_Random_Float(20.f, 40.f), DAMAGE_TYPE::ULTIMATE)
                .Interval(0.1f)
                .Charge(10.f, 0.f)
            );
        }
        else if(Event.Tag == "SawEnd")
        {
            pOwner->End_AttackCollider("Saw");
        }
    }

    auto pCorinState = pOwner->Get_StateMachine();
    if (pCorinState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentStateName() == "End")
        {
            pCorinState->Set_Trigger("SwitchOut");
            pCorinState->Set_Bool("OutReserve", false);
        }
    }

    __super::Update(pOwner, dt);
}

void CCorinState_UltimateAttack::Exit(CCorin* pOwner)
{
    pOwner->Pop_Invincible();
    __super::Exit(pOwner);
}

void CCorinState_UltimateAttack_Start::Enter(CCorin* pOwner)
{
    BattleSystem()->StartTimeScale(CBattleSystem::BATTLE_OBJ_TYPE::MONSTER, 2.f, 0.f);
    BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::ULTIMATE);
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_Start")
        //.Speed(2.f)
        .Apply();
}

void CCorinState_UltimateAttack_Start::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_UltimateAttack_Start::Exit(CCorin* pOwner)
{
}

void CCorinState_UltimateAttack_Loop::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex")
        //.Speed(2.f)
        .Apply();
}

void CCorinState_UltimateAttack_Loop::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_UltimateAttack_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_End")
        //.Speed(2.f)
        .EndAt(0.85f)
        .Apply();

    pOwner->Unlock_Move();
}

void CCorinState_UltimateAttack_End::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}
