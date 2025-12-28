#include "pch.h"
#include "MiyabiState_Walk.h"
#include "Miyabi.h"

#include "CharacterController.h"

void CMiyabiState_Walk::Enter(CMiyabi* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
        m_pSubStateMachine->Register_State("Start", CMiyabiState_Walk_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CMiyabiState_Walk_Loop::Create());
        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CMiyabiState_Walk::Update(CMiyabi* pOwner, _float dt)
{
    if (m_pSubStateMachine)
        m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());

    __super::Update(pOwner, dt);
 
}

void CMiyabiState_Walk::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_Walk_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Walk_Start");
}

void CMiyabiState_Walk_Start::Update(CMiyabi* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);

        //_vector vRootMotionDelta = pOwner->Get_Animator()->Get_RootMotionDelta(0);
        //_vector3 vDelta = vRootMotionDelta;
        //
        //if (vDelta.x != 0.f || vDelta.z != 0.f)
        //{
        //    _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
        //    pOwner->Get_CCT()->Move_RootMotion(vRootMotionDelta, qRot, dt);
        //}
    }
}

void CMiyabiState_Walk_Start::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_Walk_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Walk")
        .Loop(true)
        .Apply();
}

void CMiyabiState_Walk_Loop::Update(CMiyabi* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);

        //_vector vRootMotionDelta = pOwner->Get_Animator()->Get_RootMotionDelta(0);
        //_vector3 vDelta = vRootMotionDelta;
        //
        //if (vDelta.x != 0.f || vDelta.z != 0.f)
        //{
        //    _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
        //    pOwner->Get_CCT()->Move_RootMotion(vRootMotionDelta, qRot, dt);
        //}
    }
}

void CMiyabiState_Walk_Loop::Exit(CMiyabi* pOwner)
{
}
