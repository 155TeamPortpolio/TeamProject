#include "pch.h"
#include "CorinState_RushAttack.h"
#include "Corin.h"

CCorinState_RushAttack* CCorinState_RushAttack::Create()
{
    auto pInstance = new CCorinState_RushAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Rush_Start", CCorinState_Rush_Start::Create());
    pSubStateMachine->Register_State("Rush_Explode", CCorinState_Rush_Explode::Create());
    pSubStateMachine->Register_State("Rush_End", CCorinState_Rush_End::Create());

    pSubStateMachine->Get_State("Rush_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("Rush_Start", "Rush_Explode",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Rush_Explode", "Rush_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("Rush_Start");

    return pInstance;
}

void CCorinState_RushAttack::Enter(CCorin* pOwner)
{
    pOwner->Lock_Move();
    __super::Enter(pOwner);
}

void CCorinState_RushAttack::Update(CCorin* pOwner, _float dt)
{
    auto pCorinState = pOwner->Get_StateMachine();
    if (pCorinState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentStateName() == "Rush_Start" ||
            m_pSubStateMachine->Get_CurrentStateName() == "Rush_End" ||
            Is_AnimEnd())
        {
            pCorinState->Set_Trigger("SwitchOut");
            pCorinState->Set_Bool("OutReserve", false);
        }
    }

    __super::Update(pOwner, dt);
}

void CCorinState_RushAttack::Exit(CCorin* pOwner)
{
    __super::Exit(pOwner);
}

void CCorinState_Rush_Start::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush")
        .Speed(2.f)
        .Apply();
    pOwner->Begin_AttackCollider("Saw", HitDesc()
        .Type(HIT_TYPE::COUNT)
        .Damage(pOwner->Get_AttackPower() * 0.967f * Helper::Get_Random_Float(1.f, 1.5f)
            , DAMAGE_TYPE::NORMAL)
        .Interval(0.05f)
        .MaxCount(7)
        .Charge(1.f, 10.f)
    );
}

void CCorinState_Rush_Start::Update(CCorin* pOwner, _float dt)
{
    CCorin::ROOTMOTION_DESC desc;
    desc.iModeMask = ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION);
    desc.fMoveWeight = 0.5f;
    pOwner->Process_RootMotion(dt, desc);
}

void CCorinState_Rush_Explode::Enter(CCorin* pOwner)
{
    pOwner->End_AttackCollider("Saw");
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_Explode")
        .Speed(2.f)
        .Apply();
}

void CCorinState_Rush_Explode::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_Rush_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_End")
        .Speed(2.f)
        .Apply();
    pOwner->Unlock_Move();
}
