#include "pch.h"
#include "MiyabiState_Attack.h"
#include "MiyabiState_NormalAttack.h"
#include "Miyabi.h"

void CMiyabiState_Attack::Enter(CMiyabi* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CMiyabi>::Create();

        m_pSubStateMachine->Register_State("NormalAttack", CMiyabiState_NormalAttack::Create());
        m_pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");

        m_pSubStateMachine->Set_DefaultState("NormalAttack");
    }

    __super::Enter(pOwner);
}

void CMiyabiState_Attack::Update(CMiyabi* pOwner, _float dt)
{
    __super::Update(pOwner, dt);

    if (m_pSubStateMachine)
    {
        IBaseState<CMiyabi>* pCurrent = m_pSubStateMachine->Get_CurrentState();
        if (pCurrent && pCurrent->Get_AnimProgress() >= 1.f)
            m_fAnimProgress = 1.f;
        else
            m_fAnimProgress = 0.f;
    }
}

void CMiyabiState_Attack::Exit(CMiyabi* pOwner)
{
}