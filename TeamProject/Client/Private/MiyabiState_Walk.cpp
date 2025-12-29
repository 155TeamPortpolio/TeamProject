#include "pch.h"
#include "MiyabiState_Walk.h"
#include "Miyabi.h"
#include "MiyabiState_Move.h"

#include "CharacterController.h"

void CMiyabiState_Walk::Enter(CMiyabi* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
        m_pSubStateMachine->Register_State("Start", CMiyabiState_Walk_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CMiyabiState_Walk_Loop::Create());
        m_pSubStateMachine->Register_State("End", CMiyabiState_Walk_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CMiyabiState_Walk::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());
 
}

void CMiyabiState_Walk::Exit(CMiyabi* pOwner)
{
    CMiyabiState_Move* pMove = static_cast<CMiyabiState_Move*>(this->Get_ParentState());
    if (pMove && pMove->Get_SubStateMachine())
    {
        pMove->Get_SubStateMachine()->Set_Bool("WalkFinish", false);
    }
}

void CMiyabiState_Walk_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Walk_Start")
        .Apply();
}

void CMiyabiState_Walk_Start::Update(CMiyabi* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);

        _vector3 vRootMotionDelta = pOwner->Get_Animator()->Get_RootBoneDelta() * -1.f;
        _vector3 vDelta = vRootMotionDelta;
        
        if (vDelta.x != 0.f || vDelta.z != 0.f)
        {
            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
            pOwner->Get_CCT()->Move_RootMotion(vRootMotionDelta, qRot, dt);
        }
    }
}

void CMiyabiState_Walk_Start::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_Walk_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Walk")
        .Apply();
}

void CMiyabiState_Walk_Loop::Update(CMiyabi* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);

        _vector3 vRootMotionDelta = pOwner->Get_Animator()->Get_RootBoneDelta() * -1.f;
        _vector3 vDelta = vRootMotionDelta;

        if (vDelta.x != 0.f || vDelta.z != 0.f)
        {
            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
            pOwner->Get_CCT()->Move_RootMotion(vRootMotionDelta, qRot, dt);
        }
    }

    if (m_fAnimProgress >= 1.f)
    {
        CMiyabiState_Walk* pWalk = static_cast<CMiyabiState_Walk*>(this->Get_ParentState());
        if (pWalk)
        {
            CMiyabiState_Move* pMove = static_cast<CMiyabiState_Move*>(pWalk->Get_ParentState());
            if (pMove && pMove->Get_SubStateMachine())
            {
                pMove->Get_SubStateMachine()->Set_Bool("WalkFinish", true);
            }
        }
    }
}

void CMiyabiState_Walk_Loop::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_Walk_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Unagi_Ani_Run_Start_End")
        .Apply();
}

void CMiyabiState_Walk_End::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Walk_End::Exit(CMiyabi* pOwner)
{
}
