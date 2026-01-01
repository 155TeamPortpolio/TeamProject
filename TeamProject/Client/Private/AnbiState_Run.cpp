#include "pch.h"
#include "AnbiState_Run.h"
#include "Anbi.h"

#include "CharacterController.h"

void CAnbiState_Run::Enter(CAnbi* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CAnbi>::Create();
        m_pSubStateMachine->Register_State("Start", CAnbiState_Run_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CAnbiState_Run_Loop::Create());
        m_pSubStateMachine->Register_State("End", CAnbiState_Run_End::Create());
        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        m_pSubStateMachine->Register_Transition("Start", "Loop",
            CStateMachine<CAnbi>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Start", "End",
            CStateMachine<CAnbi>::CONDITION_BOOL_FALSE, "IsMove");
        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CAnbi>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Set_DefaultState("Start");
    }

    if (m_pSubStateMachine)
        m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());

    __super::Enter(pOwner);
}

void CAnbiState_Run::Update(CAnbi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());
}

void CAnbiState_Run_Start::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Run_Start")
        .Apply();
}

void CAnbiState_Run_Start::Update(CAnbi* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);
        _vector3 vRootMotionDelta = pOwner->Get_Animator()->Get_RootBoneMoveDelta();

        if (vRootMotionDelta.x != 0.f || vRootMotionDelta.z != 0.f)
        {
            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
            pOwner->Get_CCT()->Move_RootMotion(vRootMotionDelta, qRot, dt);
        }
    }
}

void CAnbiState_Run_Loop::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Run")
        .Loop(true)
        .Apply();
}

void CAnbiState_Run_Loop::Update(CAnbi* pOwner, _float dt)
{
    _vector3 vInputDir = pOwner->Get_InputDir();
    if (vInputDir.Length() > 0.01f)
    {
        vInputDir.Normalize();
        pOwner->Rotate(vInputDir);
        _vector3 vRootMotionDelta = pOwner->Get_Animator()->Get_RootBoneMoveDelta();

        if (vRootMotionDelta.x != 0.f || vRootMotionDelta.z != 0.f)
        {
            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
            pOwner->Get_CCT()->Move_RootMotion(vRootMotionDelta, qRot, dt);
        }
    }
}

void CAnbiState_Run_End::Enter(CAnbi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size02_Anbi_Ani_Run_End")
        .Apply();
}

void CAnbiState_Run_End::Update(CAnbi* pOwner, _float dt)
{
    if (pOwner->Is_Input())
    {
        m_fAnimProgress = 1.f;
    }
}