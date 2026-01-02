#include "pch.h"
#include "GameInstance.h"
#include "CorinState_Attack.h"
#include "CorinState_NormalAttack.h"
#include "Corin.h"

#include "CharacterController.h"

void CCorinState_Attack::Enter(CCorin* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CCorin>::Create();
        m_pSubStateMachine->Register_State("NormalAttack", CCorinState_NormalAttack::Create());
        m_pSubStateMachine->Get_State("NormalAttack")->Set_Tag("NormalAttack");
        m_pSubStateMachine->Set_DefaultState("NormalAttack");
    }
    __super::Enter(pOwner);
}

void CCorinState_Attack::Update(CCorin* pOwner, _float dt)
{
    __super::Update(pOwner, dt);
    Move_Motion(pOwner, dt);

    if (m_pSubStateMachine)
    {
        string strCurrentSub = m_pSubStateMachine->Get_CurrentStateName();

        if (strCurrentSub == "NormalAttack")
        {
            CCorinState_NormalAttack* pNormalAttack =
                static_cast<CCorinState_NormalAttack*>(m_pSubStateMachine->Get_State("NormalAttack"));

            if (pNormalAttack && pNormalAttack->Get_SubStateMachine())
            {
                string strNormalSub = pNormalAttack->Get_SubStateMachine()->Get_CurrentStateName();

                if (strNormalSub == "Attack_End")
                {
                    IBaseState<CCorin>* pAttackEnd =
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
    }
}

void CCorinState_Attack::Exit(CCorin* pOwner)
{
    if (m_pSubStateMachine)
    {
        IHState<CCorin>* pNormalAttack = dynamic_cast<IHState<CCorin>*>(m_pSubStateMachine->Get_State("NormalAttack"));
        if (pNormalAttack)
        {
            CCorinState_NormalAttack* pNA = static_cast<CCorinState_NormalAttack*>(pNormalAttack);
            // 콤보 인덱스 리셋 로직 (Corin용 클래스에 맞게 수정)
        }
    }
}

_bool CCorinState_Attack::Handle_Transition(CCorin* pOwner, const string& strState)
{
    if (strState == "Evade")
        return true;

    if (strState != "Attack")
    {
        if (!m_pSubStateMachine)
            return true;

        string strCurrentSub = m_pSubStateMachine->Get_CurrentStateName();

        // NormalAttack End 체크
        if (strCurrentSub == "NormalAttack")
        {
            CCorinState_NormalAttack* pNormalAttack =
                static_cast<CCorinState_NormalAttack*>(m_pSubStateMachine->Get_State("NormalAttack"));

            if (pNormalAttack && pNormalAttack->Get_SubStateMachine())
            {
                string strNormalSub = pNormalAttack->Get_SubStateMachine()->Get_CurrentStateName();
                IBaseState<CCorin>* pAttackEnd =
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
        return true;
    }
    return true;
}

void CCorinState_Attack::Move_Motion(CCorin* pOwner, _float dt)
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
