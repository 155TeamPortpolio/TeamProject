#include "pch.h"
#include "GameInstance.h"
#include "MiyabiState_Attack.h"
#include "MiyabiState_NormalAttack.h"
#include "MiyabiState_ChargeAttack.h"
#include "Miyabi.h"

void CMiyabiState_Attack::Enter(CMiyabi* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CMiyabi>::Create();

        m_pSubStateMachine->Register_State("NormalAttack", CMiyabiState_NormalAttack::Create());
        m_pSubStateMachine->Register_State("ChargeAttack", CMiyabiState_ChargeAttack::Create());

        m_pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");
        m_pSubStateMachine->Get_State("ChargeAttack")->Set_Tag("ChargeAttack");

        m_pSubStateMachine->Register_Transition("NormalAttack", "ChargeAttack",
            CStateMachine<CMiyabi>::CONDITION_TRIGGER, "StartCharge");

        m_pSubStateMachine->Set_DefaultState("NormalAttack");
    }
    __super::Enter(pOwner);
}

void CMiyabiState_Attack::Update(CMiyabi* pOwner, _float dt)
{
    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Hold(MOUSE_BTN::LB))
    {
        m_fHoldTime += dt;
        if (m_fHoldTime >= 0.3f)
            m_pSubStateMachine->Set_Trigger("StartCharge");
    }
    else
    {
        m_fHoldTime = 0.f;
    }

    __super::Update(pOwner, dt);

    if (m_pSubStateMachine)
    {
        string strCurrentSub = m_pSubStateMachine->Get_CurrentStateName();

        if (strCurrentSub == "NormalAttack")
        {
            CMiyabiState_NormalAttack* pNormalAttack =
                static_cast<CMiyabiState_NormalAttack*>(m_pSubStateMachine->Get_State("NormalAttack"));

            if (pNormalAttack && pNormalAttack->Get_SubStateMachine())
            {
                string strNormalSub = pNormalAttack->Get_SubStateMachine()->Get_CurrentStateName();

                // Attack_End의 진행도를 Attack에 전파
                if (strNormalSub == "Attack_End")
                {
                    IBaseState<CMiyabi>* pAttackEnd =
                        pNormalAttack->Get_SubStateMachine()->Get_CurrentState();

                    if (pAttackEnd && (pAttackEnd->Is_AnimEnd() || pOwner->Is_Input()))
                    {
                        m_fAnimProgress = 1.f;
                    }
                    else
                    {
                        m_fAnimProgress = 0.f;
                    }
                }
                else
                {
                    m_fAnimProgress = 0.f;
                }
            }
        }
        else if (strCurrentSub == "ChargeAttack")
        {
            IBaseState<CMiyabi>* pChargeAttack = m_pSubStateMachine->Get_State("ChargeAttack");
            if (pChargeAttack && pChargeAttack->Get_AnimProgress() >= 1.f)
                m_fAnimProgress = 1.f;
            else
                m_fAnimProgress = 0.f;
        }
    }
}

void CMiyabiState_Attack::Exit(CMiyabi* pOwner)
{
}