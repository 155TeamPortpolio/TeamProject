#include "pch.h"
#include "GameInstance.h"
#include "MiyabiState_Attack.h"
#include "MiyabiState_NormalAttack.h"
#include "MiyabiState_ChargeAttack.h"
#include "Miyabi.h"

#include "CharacterController.h"

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
    Move_Motion(pOwner, dt);

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

                if (strNormalSub == "Attack_End")
                {
                    IBaseState<CMiyabi>* pAttackEnd =
                        pNormalAttack->Get_SubStateMachine()->Get_CurrentState();

                    if (pAttackEnd && (pAttackEnd->Is_AnimEnd() || pAttackEnd->Get_AnimProgress() >= 1.f))
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
            CMiyabiState_ChargeAttack* pChargeAttack =
                static_cast<CMiyabiState_ChargeAttack*>(m_pSubStateMachine->Get_State("ChargeAttack"));

            if (pChargeAttack && pChargeAttack->Get_SubStateMachine())
            {
                string strChargeSub = pChargeAttack->Get_SubStateMachine()->Get_CurrentStateName();

                if (strChargeSub == "Charge_End")
                {
                    IBaseState<CMiyabi>* pChargeEnd =
                        pChargeAttack->Get_SubStateMachine()->Get_CurrentState();

                    if (pChargeEnd && (pChargeEnd->Is_AnimEnd() || pChargeEnd->Get_AnimProgress() >= 1.f))
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
    }
}

void CMiyabiState_Attack::Exit(CMiyabi* pOwner)
{
}

_bool CMiyabiState_Attack::Handle_Transition(CMiyabi* pOwner, const string& strState)
{
    if (strState != "Attack")
    {
        if (!m_pSubStateMachine)
            return true;

        string strCurrentSub = m_pSubStateMachine->Get_CurrentStateName();

        // NormalAttack End 체크
        if (strCurrentSub == "NormalAttack")
        {
            CMiyabiState_NormalAttack* pNormalAttack =
                static_cast<CMiyabiState_NormalAttack*>(m_pSubStateMachine->Get_State("NormalAttack"));

            if (pNormalAttack && pNormalAttack->Get_SubStateMachine())
            {
                string strNormalSub = pNormalAttack->Get_SubStateMachine()->Get_CurrentStateName();
                IBaseState<CMiyabi>* pAttackEnd =
                    pNormalAttack->Get_SubStateMachine()->Get_CurrentState();

                // Attack_End가 아니면 거부
                if (strNormalSub != "Attack_End")
                    return false;

                if (!pAttackEnd)
                    return true;

                // Attack_End에서 Idle로만 전환 허용
                if (strState == "Idle")
                {
                    // 애니메이션 끝났거나 입력이 있으면 허용
                    if (pAttackEnd->Is_AnimEnd() || pOwner->Is_Input())
                        return true;
                }

                return false;
            }
        }
        // ChargeAttack End 체크
        else if (strCurrentSub == "ChargeAttack")
        {
            CMiyabiState_ChargeAttack* pChargeAttack =
                static_cast<CMiyabiState_ChargeAttack*>(m_pSubStateMachine->Get_State("ChargeAttack"));

            if (pChargeAttack && pChargeAttack->Get_SubStateMachine())
            {
                string strChargeSub = pChargeAttack->Get_SubStateMachine()->Get_CurrentStateName();
                IBaseState<CMiyabi>* pChargeEnd =
                    pChargeAttack->Get_SubStateMachine()->Get_CurrentState();

                if (strChargeSub != "Charge_End")
                    return false;

                if (!pChargeEnd)
                    return true;

                if (strState == "Idle")
                {
                    if (pChargeEnd->Is_AnimEnd() || pOwner->Is_Input())
                        return true;
                }

                return false;
            }
        }

        return true;
    }

    return true;
}

void CMiyabiState_Attack::Move_Motion(CMiyabi* pOwner, _float dt)
{
    _vector3 vDir = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
    if (vDir.Length() > 0.01f)
    {
        vDir.Normalize();
        _vector3 vDelta = pOwner->Get_Animator()->Get_RootBoneMoveDelta();
        if (vDelta.x != 0.f || vDelta.z != 0.f)
        {
            _quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
            pOwner->Get_CCT()->Move_RootMotion(vDelta, qRot, dt);
        }
    }
}
