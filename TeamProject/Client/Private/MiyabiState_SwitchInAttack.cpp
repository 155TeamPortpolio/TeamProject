#include "pch.h"
#include "MiyabiState_SwitchInAttack.h"

#include "BattleSystem.h"

#include "Miyabi.h"

CMiyabiState_SwitchInAttack* CMiyabiState_SwitchInAttack::Create()
{
    auto pInstance = new CMiyabiState_SwitchInAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("SwitchInAttack_Start", CMiyabiState_SwitchInAttack_Start::Create());
    pSubStateMachine->Register_State("SwitchInAttack_End", CMiyabiState_SwitchInAttack_End::Create());

    pSubStateMachine->Get_State("SwitchInAttack_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("SwitchInAttack_Start", "SwitchInAttack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("SwitchInAttack_Start");

    return pInstance;
}

void CMiyabiState_SwitchInAttack::Enter(CMiyabi* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Lock_Move();

    __super::Enter(pOwner);
}

void CMiyabiState_SwitchInAttack::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Look_Target();

    auto pMiyabiState = pOwner->Get_StateMachine();
    if (pMiyabiState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" ||
            Is_AnimEnd())
        {
            pMiyabiState->Set_Trigger("SwitchOut");
            pMiyabiState->Set_Bool("OutReserve", false);
        }
    }

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        IHState<CMiyabi>* pSwitchIn = Get_ParentState();
        if (pSwitchIn && pSwitchIn->Get_SubStateMachine())
        {
            pSwitchIn->Get_SubStateMachine()->Set_Int("ExitMode", 0);  // Idle·Î
            pSwitchIn->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }

    __super::Update(pOwner, dt);
}

void CMiyabiState_SwitchInAttack::Exit(CMiyabi* pOwner)
{
    pOwner->Pop_Invincible();
    pOwner->Unlock_Move();
    __super::Exit(pOwner);
}

void CMiyabiState_SwitchInAttack_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack")
        .Loop(false)
        .Apply();

    m_vPos = pOwner->Get_WorldPos();
    m_vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
}

void CMiyabiState_SwitchInAttack_Start::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

        if (Event.Tag == "AreaAttack")
        {
            BattleSystem()->TakeAreaDamage(m_vPos + m_vLook * 7.f, 7.f, HitDesc()
                .Name(pOwner->Get_CharacterName())
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 0.897f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::HARD)
            );
        }
    }
}

void CMiyabiState_SwitchInAttack_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CMiyabiState_SwitchInAttack_End::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    IHState<CMiyabi>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
