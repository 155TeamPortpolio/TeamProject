#include "pch.h"
#include "MiyabiState_RushAttack.h"
#include "Miyabi.h"

CMiyabiState_RushAttack* CMiyabiState_RushAttack::Create()
{
    auto pInstance = new CMiyabiState_RushAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Rush_Start", CMiyabiState_Rush_Start::Create());
    pSubStateMachine->Register_State("Rush_End", CMiyabiState_Rush_End::Create());

    pSubStateMachine->Get_State("Rush_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("Rush_Start", "Rush_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("Rush_Start");

    return pInstance;
}

void CMiyabiState_RushAttack::Enter(CMiyabi* pOwner)
{
    pOwner->Lock_Move();
    __super::Enter(pOwner);
}

void CMiyabiState_RushAttack::Update(CMiyabi* pOwner, _float dt)
{
    auto pMiyabiState = pOwner->Get_StateMachine();
    if (pMiyabiState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentStateName() == "Rush_Start" ||
            m_pSubStateMachine->Get_CurrentStateName() == "Rush_End" ||
            Is_AnimEnd())
        {
            pMiyabiState->Set_Trigger("SwitchOut");
            pMiyabiState->Set_Bool("OutReserve", false);
        }
    }

    __super::Update(pOwner, dt);
}

void CMiyabiState_RushAttack::Exit(CMiyabi* pOwner)
{
    __super::Exit(pOwner);
}

void CMiyabiState_Rush_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush")
        .Speed(1.f)
        .Apply();
    pOwner->Begin_AttackCollider("KatanaWeapon", HitDesc()
        .Name(pOwner->Get_CharacterName())
        .Type(HIT_TYPE::ONCE)
        .Damage(pOwner->Get_AttackPower() * 0.258f * Helper::Get_Random_Float(1.f, 1.5f)
            , DAMAGE_TYPE::NORMAL)
    );
}

void CMiyabiState_Rush_Start::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Rush_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_End")
        .Speed(1.f)
        .Apply();
    pOwner->End_AttackCollider("KatanaWeapon");
    pOwner->Unlock_Move();
}
