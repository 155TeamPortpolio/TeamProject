#include "pch.h"
#include "MiyabiState_Run.h"
#include "Miyabi.h"

#include "CharacterController.h"

void CMiyabiState_Run::Enter(CMiyabi* pOwner)
{
    if(!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
        m_pSubStateMachine->Register_State("Start", CMiyabiState_Run_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CMiyabiState_Run_Loop::Create());
        m_pSubStateMachine->Register_State("End", CMiyabiState_Run_End::Create());
        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");
        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Set_DefaultState("Start");
    }

    if (m_pSubStateMachine)
        m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());

    __super::Enter(pOwner);
}

void CMiyabiState_Run::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());
}

void CMiyabiState_Run::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_Run_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Run_Start")
        .Apply();
}

void CMiyabiState_Run_Start::Update(CMiyabi* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);
        _vector3 vRootMotionDelta = pOwner->Get_Animator()->Get_RootMotionDelta() * -1.f;

        if (vRootMotionDelta.x != 0.f || vRootMotionDelta.z != 0.f)
        {
            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
            pOwner->Get_CCT()->Move_RootMotion(vRootMotionDelta, qRot, dt);
        }
    }
}

void CMiyabiState_Run_Start::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_Run_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Run")
        .Loop(true)
        .Apply();
}

void CMiyabiState_Run_Loop::Update(CMiyabi* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);
        _vector3 vRootMotionDelta = pOwner->Get_Animator()->Get_RootMotionDelta() * -1.f;

        if (vRootMotionDelta.x != 0.f || vRootMotionDelta.z != 0.f)
        {
            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
            pOwner->Get_CCT()->Move_RootMotion(vRootMotionDelta, qRot, dt);
        }
    }
}

void CMiyabiState_Run_Loop::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_Run_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Run_End")
        .Apply();
}

void CMiyabiState_Run_End::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Run_End::Exit(CMiyabi* pOwner)
{
}
