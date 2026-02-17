#include "pch.h"
#include "CorinState_SwitchInAttack.h"

#include "Corin.h"
#include "AudioSource.h"

CCorinState_SwitchInAttack* CCorinState_SwitchInAttack::Create()
{
    auto pInstance = new CCorinState_SwitchInAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("SwitchInAttack_Start", CCorinState_SwitchInAttack_Start::Create());
    pSubStateMachine->Register_State("SwitchInAttack_End", CCorinState_SwitchInAttack_End::Create());

    pSubStateMachine->Get_State("SwitchInAttack_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("SwitchInAttack_Start", "SwitchInAttack_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("SwitchInAttack_Start");

    return pInstance;
}

void CCorinState_SwitchInAttack::Enter(CCorin* pOwner)
{
    pOwner->Push_Invincible();
    pOwner->Lock_Move();

    __super::Enter(pOwner);
}

void CCorinState_SwitchInAttack::Update(CCorin* pOwner, _float dt)
{
    pOwner->Look_Target();

    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "SawInterval")
        {
            pOwner->Begin_AttackCollider("Saw", HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(pOwner->Get_AttackPower() * 2.225f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::HARD)
                .Interval(0.07f)
            );
        }
        else if (Event.Tag == "SawEnd")
        {
            pOwner->End_AttackCollider("Saw");
        }
    }

    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        IHState<CCorin>* pSwitchIn = Get_ParentState();
        if (pSwitchIn && pSwitchIn->Get_SubStateMachine())
        {
            pSwitchIn->Get_SubStateMachine()->Set_Int("ExitMode", 0);  // Idle·Î
            pSwitchIn->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }

    __super::Update(pOwner, dt);
}

void CCorinState_SwitchInAttack::Exit(CCorin* pOwner)
{
    pOwner->Pop_Invincible();
    pOwner->Unlock_Move();
    pOwner->Set_ResetMove(true);
    __super::Exit(pOwner);
}

void CCorinState_SwitchInAttack_Start::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_02")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CCorinState_SwitchInAttack_Start::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CCorinState_SwitchInAttack_Start::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.08f))
    {
        pOwner->Play_Effect("Corin_Saw_Slash0", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
        pOwner->Play_Effect("Corin_Ultimate_Saw_Slash0", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
    }

    if (IsCrossAnimProgress(0.27f))
        pOwner->Play_Effect("Corin_Normal_Slash0", _vector3(0.f, 0.6f, 0.f), _quaternion(0.73f, -0.04f, 0.03f, -0.68f));
    if (IsCrossAnimProgress(0.35f))
        pOwner->Play_Effect("Corin_Normal_Slash1", _vector3(0.f, 0.4f, 0.f), _quaternion(0.72f, 0.f, -0.02f, -0.69f));
    if (IsCrossAnimProgress(0.44f))
        pOwner->Play_Effect("Corin_Normal_Slash2", _vector3(0.f, 0.5f, 0.f), _quaternion(0.7f, -0.13f, 0.13f, -0.69f));
}

void CCorinState_SwitchInAttack_End::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack_02_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();

    pOwner->Stop_Effect("Corin_Saw_Slash0");
    pOwner->Stop_Effect("Corin_Ultimate_Saw_Slash0");
}

void CCorinState_SwitchInAttack_End::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    IHState<CCorin>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
