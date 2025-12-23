#include "pch.h"
#include "MiyabiState_Walk.h"
#include "Miyabi.h"

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
}

void CMiyabiState_Walk_Start::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_Walk_Loop::Enter(CMiyabi* pOwner)
{
    OutputDebugStringA("Walk_Loop Enter\n");
    pOwner->Get_Animator()->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Walk")
        .Loop(true)
        .Apply();
}

void CMiyabiState_Walk_Loop::Update(CMiyabi* pOwner, _float dt)
{
    OutputDebugStringA(("Walk_Loop Progress: " + to_string(m_fAnimProgress) + "\n").c_str());
}

void CMiyabiState_Walk_Loop::Exit(CMiyabi* pOwner)
{
    OutputDebugStringA("Walk Exit\n");
}
