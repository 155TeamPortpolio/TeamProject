#include "pch.h"
#include "CorinState_Walk.h"

#include "Corin.h"
#include "CorinState_Move.h"

#include "CharacterController.h"



void CCorinState_Walk::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("Start", CCorinState_Walk_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CCorinState_Walk_Loop::Create());
        m_pSubStateMachine->Register_State("End", CCorinState_Walk_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CCorin>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CCorinState_Walk::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());
}

void CCorinState_Walk_Start::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Corin_Ani_Walk_Start").Loop(false).Speed(1.2f).Apply();
}

void CCorinState_Walk_Start::Update(CCorin* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);

        _vector3 vDelta = pOwner->Get_Animator()->Get_RootBoneMoveDelta();
        if (vDelta.x != 0.f || vDelta.z != 0.f)
        {
            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
            pOwner->Get_CCT()->Move_RootMotion(vDelta, qRot, dt);
        }
    }
}

void CCorinState_Walk_Loop::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Corin_Ani_Walk")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_Walk_Loop::Update(CCorin* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);

        _vector3 vDelta = pOwner->Get_Animator()->Get_RootBoneMoveDelta();
        if (vDelta.x != 0.f || vDelta.z != 0.f)
        {
            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
            pOwner->Get_CCT()->Move_RootMotion(vDelta, qRot, dt);
        }
    }

    if (m_fAnimProgress >= 1.f)
    {
        CCorinState_Walk* pWalk = static_cast<CCorinState_Walk*>(this->Get_ParentState());
        if (pWalk)
        {
            CCorinState_Move* pMove = static_cast<CCorinState_Move*>(pWalk->Get_ParentState());
            if (pMove && pMove->Get_SubStateMachine())
            {
                pMove->Get_SubStateMachine()->Set_Trigger("ToRun");
            }
        }
    }
}

void CCorinState_Walk_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Corin_Ani_Run_Start_End")
        .Apply();
}
