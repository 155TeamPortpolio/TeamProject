#include "pch.h"
#include "CorinState_SwitchInAttack.h"

#include "Corin.h"

CCorinState_SwitchInAttack* CCorinState_SwitchInAttack::Create()
{
    auto pInstance = new CCorinState_SwitchInAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("SwitchInAttack_Start", CCorinState_SwitchInAttack_Start::Create());
    pSubStateMachine->Register_State("SwitchInAttack_End", CCorinState_SwitchInAttack_End::Create());

    pSubStateMachine->Get_State("SwitchInAttack_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("SwitchInAttack_Start", "SwitchInAttack_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("SwitchInAttack_Start");

    return pInstance;
}

void CCorinState_SwitchInAttack::Enter(CCorin* pOwner)
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

void CCorinState_SwitchInAttack::Update(CCorin* pOwner, _float dt)
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

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        IHState<CCorin>* pSwitchIn = Get_ParentState();
        if (pSwitchIn && pSwitchIn->Get_SubStateMachine())
        {
            pSwitchIn->Get_SubStateMachine()->Set_Int("ExitMode", 0);  // Idle·Î
            pSwitchIn->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }
    __super::Update(pOwner, dt);
}

void CCorinState_SwitchInAttack::Exit(CCorin* pOwner)
{
    pOwner->Pop_Invincible();
    pOwner->Unlock_Move();
    __super::Exit(pOwner);
}

void CCorinState_SwitchInAttack_Start::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_02")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_SwitchInAttack_Start::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_SwitchInAttack_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_02_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_SwitchInAttack_End::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    IHState<CCorin>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
