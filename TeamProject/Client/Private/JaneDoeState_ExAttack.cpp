#include "pch.h"
#include "JaneDoeState_ExAttack.h"
#include "JaneDoe.h"

void CJaneDoeState_ExAttack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
        
        m_pSubStateMachine->Register_State("Start", CJaneDoeState_ExAttack_Start::Create());
        m_pSubStateMachine->Register_State("End", CJaneDoeState_ExAttack_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CJaneDoeState_ExAttack::Update(CJaneDoe* pOwner, _float dt)
{
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

    __super::Update(pOwner, dt);
}

void CJaneDoeState_ExAttack::Exit(CJaneDoe* pOwner)
{
    __super::Exit(pOwner);
}

void CJaneDoeState_ExAttack_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ExSpecial")
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_ExAttack_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_ExAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ExSpecial_End")
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_ExAttack_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}
