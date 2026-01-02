#include "pch.h"
#include "CorinState_Run.h"
#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Run::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        //m_pSubStateMachine->Register_State("Start", CCorinState_Run_Start::Create());
        m_pSubStateMachine->Register_State("Loop", CCorinState_Run_Loop::Create());
        m_pSubStateMachine->Register_State("End", CCorinState_Run_End::Create());
        m_pSubStateMachine->Get_State("End")->Set_Tag("End");

        //m_pSubStateMachine->Register_Transition("Start", "Loop",
        //    CStateMachine<CCorin>::CONDITION_ANIMATION_END);
        //m_pSubStateMachine->Register_Transition("Start", "End",
        //    CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");
        m_pSubStateMachine->Register_Transition("Loop", "End",
            CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "IsMove");

        m_pSubStateMachine->Set_DefaultState("Loop");
    }

    if (m_pSubStateMachine)
        m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());

    __super::Enter(pOwner);
}

void CCorinState_Run::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    m_pSubStateMachine->Set_Bool("IsMove", pOwner->Is_Move());
}

//void CCorinState_Run_Start::Enter(CCorin* pOwner)
//{
//    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Run_Start")
//        .Apply();
//}
//
//void CCorinState_Run_Start::Update(CCorin* pOwner, _float dt)
//{
//    _vector3 vInputDir = pOwner->Get_InputDir();
//    if (vInputDir.Length() > 0.01f)
//    {
//        vInputDir.Normalize();
//        pOwner->Rotate(vInputDir);
//        _vector3 vRootMotionDelta = pOwner->Get_Animator()->Get_RootBoneMoveDelta();
//
//        if (vRootMotionDelta.x != 0.f || vRootMotionDelta.z != 0.f)
//        {
//            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
//            pOwner->Get_CCT()->Move_RootMotion(vRootMotionDelta, qRot, dt);
//        }
//    }
//}

void CCorinState_Run_Loop::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Run")
        .Loop(true)
        .Apply();
}

void CCorinState_Run_Loop::Update(CCorin* pOwner, _float dt)
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

void CCorinState_Run_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation("Avatar_Female_Size01_Corin_Ani_Run_End")
        .Apply();
}

void CCorinState_Run_End::Update(CCorin* pOwner, _float dt)
{
    if (pOwner->Is_Input())
    {
        m_fAnimProgress = 1.f;
    }
}