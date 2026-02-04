#include "pch.h"
#include "CorinState_UltimateAttack.h"

#include "BattleSystem.h"
#include "CamDirector.h"

#include "Corin.h"

CCorinState_UltimateAttack* CCorinState_UltimateAttack::Create()
{
    auto pInstance = new CCorinState_UltimateAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();


    pSubStateMachine->Register_State("Ultimate_Start", CCorinState_UltimateAttack_Start::Create());
    pSubStateMachine->Register_State("Ultimate_Loop", CCorinState_UltimateAttack_Loop::Create());
    pSubStateMachine->Register_State("Ultimate_End", CCorinState_UltimateAttack_End::Create());

    pSubStateMachine->Get_State("Ultimate_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("Ultimate_Start", "Ultimate_Loop",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Ultimate_Loop", "Ultimate_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("Ultimate_Start");

    return pInstance;
}

void CCorinState_UltimateAttack::Enter(CCorin* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Lock_Move();
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
                .Damage(pOwner->Get_AttackPower() * 20.288f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::ULTIMATE)
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
    BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::ULTIMATE);
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_Ex_Start")
        //.Speed(2.f)
        .Apply();

    pOwner->Play_Effect("Corin_Saw_Slash0", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
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

    pOwner->Play_Effect("Corin_Ex_Saw_Slash0", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
}

void CCorinState_UltimateAttack_Loop::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CCorinState_UltimateAttack_Loop::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.22f))
        pOwner->Play_Effect("Corin_Normal_Slash0", _vector3(0.f, 0.5f, 0.f), _quaternion(-0.68f, 0.17f, 0.15f, 0.7f));
    if (IsCrossAnimProgress(0.31f))
        pOwner->Play_Effect("Corin_Normal_Slash1", _vector3(0.f, 0.6f, 0.f), _quaternion(-0.69f, 0.13f, 0.19f, 0.69f));
    if (IsCrossAnimProgress(0.41f))
        pOwner->Play_Effect("Corin_Normal_Slash2", _vector3(0.f, 0.5f, 0.f), _quaternion(-0.62f, 0.25f, 0.08f, 0.74f));
    if (IsCrossAnimProgress(0.69f))
        pOwner->Play_Effect("Corin_Normal_Slash3", _vector3(-0.1f, 1.2f, 0.6f), _quaternion(0.72f, 0.14f, 0.66f, 0.15f));
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

    Update_Effects(pOwner);
}

void CCorinState_UltimateAttack_End::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.2f))
    {
        pOwner->Stop_Effect("Corin_Saw_Slash0");
        pOwner->Stop_Effect("Corin_Ex_Saw_Slash0");
    }
}
