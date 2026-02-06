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
    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

        if (Event.Tag == "LFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_L", HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(pOwner->Get_AttackPower() * 3.008f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::HARD)
                .Interval(0.05f)
            );
        }
        else if (Event.Tag == "LFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_L");
        }
        else if (Event.Tag == "RFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_R", HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(pOwner->Get_AttackPower() * 3.008f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::HARD)
                .Interval(0.05f)
            );
        }
        else if (Event.Tag == "RFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_R");
        }
    }

    auto pJaneDoeState = pOwner->Get_StateMachine();
    if (pJaneDoeState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentStateName() == "End" ||
            Is_AnimEnd())
        {
            pJaneDoeState->Set_Trigger("SwitchOut");
            pJaneDoeState->Set_Bool("OutReserve", false);
        }
    }
    __super::Update(pOwner, dt);
}

void CJaneDoeState_BranchAttack::Exit(CJaneDoe* pOwner)
{
    pOwner->Reset_ReserveCombo();
    __super::Exit(pOwner);
}

void CJaneDoeState_BranchAttack_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_01")
        .Apply();

    m_fRepeatProgress = 0.4f;
    m_iRepeatCount = 0;
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

    Update_Effects(pOwner);
}

void CJaneDoeState_BranchAttack_Start::Update_Effects(CJaneDoe* pOwner)
{
    if (IsCrossAnimProgress(0.17f))
        pOwner->Play_Effect("JaneDoe_Normal_Slash0", _vector3(-0.3f, 1.1f, 0.1f), _quaternion(0.63f, 0.07f, -0.26f, 0.73f));
    if (IsCrossAnimProgress(0.29f))
        pOwner->Play_Effect("JaneDoe_Normal_Slash1", _vector3(-0.3f, 1.1f, 0.1f), _quaternion(0.58f, 0.29f, -0.13f, 0.75f));

    if (m_iRepeatCount < 12)
    {
        if (IsCrossAnimProgress(m_fRepeatProgress))
        {
            pOwner->Play_Effect("JaneDoe_Ex_Slash" + to_string(m_iRepeatCount % 3), _vector3(0.f, 0.8f, 0.f), _quaternion(0.25f, 0.66f, -0.66f, 0.25f));

            m_fRepeatProgress += m_fRepeatInterval;
            ++m_iRepeatCount;
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

    Update_Effects(pOwner);
}

void CJaneDoeState_BranchAttack_Release01::Update_Effects(CJaneDoe* pOwner)
{
    if (IsCrossAnimProgress(0.1f))
        pOwner->Play_Effect("JaneDoe_Normal_Slash0", _vector3(0.f, 1.6f, 0.f), _quaternion(0.69f, -0.17f, -0.69f, 0.17f));
    if (IsCrossAnimProgress(0.32f))
        pOwner->Play_Effect("JaneDoe_Normal_Slash1", _vector3(0.f, 0.f, 0.9f), _quaternion(-0.06f, 0.69f, 0.72f, 0.05f));
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
