#include "pch.h"
#include "JaneDoeState_SwitchInAttack.h"

#include "BattleSystem.h"

#include "JaneDoe.h"
#include "CamDirector.h"

CJaneDoeState_SwitchInAttack* CJaneDoeState_SwitchInAttack::Create()
{
    auto pInstance = new CJaneDoeState_SwitchInAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("SwitchInAttack_Start", CJaneDoeState_SwitchInAttack_Start::Create());
    pSubStateMachine->Register_State("SwitchInAttack_End", CJaneDoeState_SwitchInAttack_End::Create());

    pSubStateMachine->Get_State("SwitchInAttack_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("SwitchInAttack_Start", "SwitchInAttack_End",
        CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("SwitchInAttack_Start");

    return pInstance;
}

void CJaneDoeState_SwitchInAttack::Enter(CJaneDoe* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Lock_Move();

    __super::Enter(pOwner);
}

void CJaneDoeState_SwitchInAttack::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Look_Target();

    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "AreaAttack")
        {
            _vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
            _vector3 vPos = pOwner->Get_WorldPos();
            BattleSystem()->TakeAreaDamage(vPos + vLook * 4.f, 8.f, HitDesc()
                .Name(pOwner->Get_CharacterName())
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 0.451f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::SWITCH)
            );
        }
        else if (Event.Tag == "AreaAttackHard")
        {
            _vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
            _vector3 vPos = pOwner->Get_WorldPos();
            BattleSystem()->TakeAreaDamage(vPos + vLook * 4.f, 8.f, HitDesc()
                .Name(pOwner->Get_CharacterName())
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 0.451f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::SWITCH)
            );
        }
    }

    auto pJaneDoeState = pOwner->Get_StateMachine();
    if (pJaneDoeState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentState()->Get_Tag() == "End" ||
            Is_AnimEnd())
        {
            pJaneDoeState->Set_Trigger("SwitchOut");
            pJaneDoeState->Set_Bool("OutReserve", false);
        }
    }

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        IHState<CJaneDoe>* pSwitchIn = Get_ParentState();
        if (pSwitchIn && pSwitchIn->Get_SubStateMachine())
        {
            pSwitchIn->Get_SubStateMachine()->Set_Int("ExitMode", 0);  // Idle·Î
            pSwitchIn->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }

    __super::Update(pOwner, dt);
}

void CJaneDoeState_SwitchInAttack::Exit(CJaneDoe* pOwner)
{
    pOwner->Pop_Invincible();
    pOwner->Unlock_Move();
    pOwner->Set_ResetMove(true);
    __super::Exit(pOwner);
}

void CJaneDoeState_SwitchInAttack_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack")
        .Loop(false)
        .Speed(1.f)
        .ReserveSpeed(0.6f, 0.8f, 0.5f, EaseType::OutBack)
        .ReserveSpeed(0.8f, 1.f, 1.f, EaseType::OutQuint)
        .Apply();
}

void CJaneDoeState_SwitchInAttack_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    if (IsCrossAnimProgress(0.65f))
    {
        if (pOwner->Get_CurrentTutorial() == TUTORIAL_TYPE::GROGGY_COMBO)
        {
            TUTORIAL_ACTION_DESC desc;
            desc.eAction = TUTORIAL_ACTION::COMBO;
            EventSystem()->Broadcast<TUTORIAL_ACTION_DESC>(desc);
        }
    }

    Update_Effects(pOwner);
}

void CJaneDoeState_SwitchInAttack_Start::Update_Effects(CJaneDoe* pOwner)
{
    if (IsCrossAnimProgress(0.29f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash0_0", _vector3(0.f, 1.9f, 0.f), _quaternion(-0.1f, 0.94f, 0.03f, -0.33f));
    if (IsCrossAnimProgress(0.35f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash0_1", _vector3(2.8f, 3.6f, 0.8f), _quaternion(0.93f, 0.24f, -0.01f, -0.28f));
    if (IsCrossAnimProgress(0.4f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash0_0", _vector3(0.f, 0.8f, 0.f), _quaternion(0.f, 0.94f, 0.f, 0.33f));
    if (IsCrossAnimProgress(0.45f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash1_0", _vector3(-1.1f, 1.8f, 2.4f), _quaternion(-0.61f, 0.05f, -0.4f, 0.68f));
    if (IsCrossAnimProgress(0.52f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash0_0", _vector3(1.7f, 2.8f, 5.f), _quaternion(0.21f, 0.84f, 0.01f, 0.5f));
    if (IsCrossAnimProgress(0.55f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash3_0", _vector3(0.7f, -0.3f, 7.6f), _quaternion(0.f, 0.f, 0.f, 1.f));
    if (IsCrossAnimProgress(0.63f))
        pOwner->Play_Effect("JaneDoe_Ultimate_Slash2_0", _vector3(-0.2f, 4.2f, 3.2f), _quaternion(-0.43f, 0.57f, -0.44f, 0.55f));
}

void CJaneDoeState_SwitchInAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CJaneDoeState_SwitchInAttack_End::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    IHState<CJaneDoe>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
