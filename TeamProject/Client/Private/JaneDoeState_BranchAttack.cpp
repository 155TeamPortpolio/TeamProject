#include "pch.h"
#include "JaneDoeState_BranchAttack.h"
#include "JaneDoe.h"

#include "GameInstance.h"

void CJaneDoeState_BranchAttack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();

        m_pSubStateMachine->Register_State("Start", CJaneDoeState_BranchAttack_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CJaneDoeState_BranchAttack_Loop::Create());
        m_pSubStateMachine->Register_State("Release01", CJaneDoeState_BranchAttack_Release01::Create());
        m_pSubStateMachine->Register_State("Release02", CJaneDoeState_BranchAttack_Release02::Create());
        m_pSubStateMachine->Register_State("End", CJaneDoeState_BranchAttack_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Release01",
            CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "Release");
        m_pSubStateMachine->Register_Transition("Release01", "End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CJaneDoeState_BranchAttack::Update(CJaneDoe* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

        if (Event.Tag == "LFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_L", { HIT_TYPE::INTERVAL, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(30,60), 0.05, 0 });
        }
        else if (Event.Tag == "LFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_L");
        }
        else if (Event.Tag == "RFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_R", { HIT_TYPE::INTERVAL, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(30,60), 0.05, 0 });
        }
        else if (Event.Tag == "RFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_R");
        }
    }
}

void CJaneDoeState_BranchAttack::Exit(CJaneDoe* pOwner)
{
    __super::Exit(pOwner);
}

void CJaneDoeState_BranchAttack_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_01")
        .Apply();
}

void CJaneDoeState_BranchAttack_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Hold(MOUSE_BTN::LB) == false
        || m_fAnimProgress >= 1.f)
    {
        IHState<CJaneDoe>* pAttackState = Get_ParentState();
        if (pAttackState && pAttackState->Get_SubStateMachine())
        {
            pAttackState->Get_SubStateMachine()->Set_Trigger("Release");
        }
    }
}

void CJaneDoeState_BranchAttack_Loop::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_02_Repeat")
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_BranchAttack_Loop::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_BranchAttack_Release01::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_01_Release")
        //.Speed(1.2f)
        .Apply();
}

void CJaneDoeState_BranchAttack_Release01::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_BranchAttack_Release02::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_02_Release")
        .Apply();
}

void CJaneDoeState_BranchAttack_Release02::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_BranchAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_01_End")
        .Apply();
}

void CJaneDoeState_BranchAttack_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}
