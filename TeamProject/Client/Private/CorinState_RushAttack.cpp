#include "pch.h"
#include "CorinState_RushAttack.h"
#include "Corin.h"

void CCorinState_RushAttack::Enter(CCorin* pOwner)
{
    pOwner->Lock_Move();
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Rush_Start", CCorinState_Rush_Start::Create());
        m_pSubStateMachine->Register_State("Rush_Explode", CCorinState_Rush_Explode::Create());
        m_pSubStateMachine->Register_State("Rush_End", CCorinState_Rush_End::Create());
        
        m_pSubStateMachine->Get_State("Rush_End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Rush_Start", "Rush_Explode",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Rush_Explode", "Rush_End",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Rush_Start");
    }
    __super::Enter(pOwner);
}

void CCorinState_RushAttack::Update(CCorin* pOwner, _float dt)
{
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
    pOwner->Begin_AttackCollider("Saw", { HIT_TYPE::COUNT, DAMAGE_TYPE::NORMAL, 1.f, 0.f, 7 });
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
