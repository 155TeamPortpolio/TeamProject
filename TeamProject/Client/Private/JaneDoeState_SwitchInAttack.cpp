#include "pch.h"
#include "JaneDoeState_SwitchInAttack.h"

#include "JaneDoe.h"

CJaneDoeState_SwitchInAttack* CJaneDoeState_SwitchInAttack::Create()
{
    auto pInstance = new CJaneDoeState_SwitchInAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("SwitchInAttack_Start", CJaneDoeState_SwitchInAttack_Start::Create());
    pSubStateMachine->Register_State("SwitchInAttack_End", CJaneDoeState_SwitchInAttack_End::Create());

    pSubStateMachine->Get_State("SwitchInAttack_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("SwitchInAttack_Start", "SwitchInAttack_End",
        CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("SwitchInAttack_Start");

    return pInstance;
}

void CJaneDoeState_SwitchInAttack::Enter(CJaneDoe* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Lock_Move();
    if (pOwner->Get_TargetHandle().isValid())
    {
        auto target = pOwner->Get_TargetHandle().Get();
        _vector3 vLook = target->Get_WorldPos() - pOwner->Get_WorldPos();
        vLook.y = 0;
        vLook.Normalize();
        pOwner->Get_Component<CTransform>()->Set_Look(vLook);
        pOwner->Rotate(vLook);
    }
    __super::Enter(pOwner);
}

void CJaneDoeState_SwitchInAttack::Update(CJaneDoe* pOwner, _float dt)
{
    if (pOwner->Get_TargetHandle().isValid())
    {
        auto target = pOwner->Get_TargetHandle().Get();
        _vector3 vLook = target->Get_WorldPos() - pOwner->Get_WorldPos();
        vLook.y = 0;
        vLook.Normalize();
        pOwner->Get_Component<CTransform>()->Set_Look(vLook);
        pOwner->Rotate(vLook);
    }

    auto pJaneDoeState = pOwner->Get_StateMachine();
    if (pJaneDoeState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" ||
            Is_AnimEnd())
        {
            pJaneDoeState->Set_Trigger("SwitchOut");
            pJaneDoeState->Set_Bool("OutReserve", false);
        }
    }

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        IHState<CJaneDoe>* pSwitchIn = Get_ParentState();
        if (pSwitchIn && pSwitchIn->Get_SubStateMachine())
        {
            pSwitchIn->Get_SubStateMachine()->Set_Int("ExitMode", 0);  // Idle·Î
            pSwitchIn->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }

    __super::Update(pOwner, dt);
}

void CJaneDoeState_SwitchInAttack::Exit(CJaneDoe* pOwner)
{
    pOwner->Pop_Invincible();
    pOwner->Unlock_Move();
    __super::Exit(pOwner);
}

void CJaneDoeState_SwitchInAttack_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInAttack_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_SwitchInAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInAttack_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    IHState<CJaneDoe>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
