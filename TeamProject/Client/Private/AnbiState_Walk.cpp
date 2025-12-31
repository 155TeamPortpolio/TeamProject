#include "pch.h"
#include "AnbiState_Walk.h"

#include "Anbi.h"
#include "AnbiState_Move.h"

#include "CharacterController.h"



void CAnbiState_Walk::Enter(CAnbi* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CAnbi>::Create();
        m_pSubStateMachine->Register_State("Start", CAnbiState_Walk_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CAnbiState_Walk_Loop::Create());
        m_pSubStateMachine->Register_State("End", CAnbiState_Walk_End::Create());

        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CAnbi>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CAnbi>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CAnbi>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Set_DefaultState("Start");
    }
    __super::Enter(pOwner);
}

void CAnbiState_Walk::Update(CAnbi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());
}

void CAnbiState_Walk_Start::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Walk_Start").Loop(false).Speed(1.2f).Apply();
}

void CAnbiState_Walk_Start::Update(CAnbi* pOwner, _float dt)
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

void CAnbiState_Walk_Loop::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Walk")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CAnbiState_Walk_Loop::Update(CAnbi* pOwner, _float dt)
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
        CAnbiState_Walk* pWalk = static_cast<CAnbiState_Walk*>(this->Get_ParentState());
        if (pWalk)
        {
            CAnbiState_Move* pMove = static_cast<CAnbiState_Move*>(pWalk->Get_ParentState());
            if (pMove && pMove->Get_SubStateMachine())
            {
                pMove->Get_SubStateMachine()->Set_Trigger("ToRun");
            }
        }
    }
}

void CAnbiState_Walk_End::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Run_Start_End")
        .Apply();
}
