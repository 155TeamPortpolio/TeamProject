#include "pch.h"
#include "GameInstance.h"
#include "CorinState_ExAttack.h"
#include "Corin.h"

CCorinState_ExAttack* CCorinState_ExAttack::Create()
{
    auto pInstance = new CCorinState_ExAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("ExAttack_Start", CCorinState_ExAttack_Start::Create());
    pSubStateMachine->Register_State("ExAttack_Loop", CCorinState_ExAttack_Loop::Create());
    pSubStateMachine->Register_State("ExAttack_Loop_Walk", CCorinState_ExAttack_Loop_Walk::Create());
    pSubStateMachine->Register_State("ExAttack_Explode", CCorinState_ExAttack_Explode::Create());
    pSubStateMachine->Register_State("ExAttack_End", CCorinState_ExAttack_End::Create());

    pSubStateMachine->Get_State("ExAttack_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("ExAttack_Start", "ExAttack_Loop",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("ExAttack_Loop", "ExAttack_Loop_Walk",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToWalk");
    pSubStateMachine->Register_AnyStateTransition("ExAttack_Explode",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToExplode");
    pSubStateMachine->Register_Transition("ExAttack_Explode", "ExAttack_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("ExAttack_Start");

    return pInstance;
}

void CCorinState_ExAttack::Enter(CCorin* pOwner)
{
    // 강화 상태 판정
    auto tDesc = pOwner->Get_EnergyDesc();
    _bool bEnhanced = tDesc.fCurrentEnergy >= tDesc.fSpecialEnergy;
    m_pSubStateMachine->Set_Bool("Enhanced", bEnhanced);
    // current, special 변경
    if (bEnhanced)
    {
        pOwner->Get_StateMachine()->Set_Bool("Resistance", true);
        pOwner->Set_CurrentEnergy(tDesc.fCurrentEnergy - 20.f);
        pOwner->Set_SpecialEnergy(20.f);
    }
    //초기화
    m_pSubStateMachine->Set_Bool("ExFinished", false);

    __super::Enter(pOwner);
}

void CCorinState_ExAttack::Update(CCorin* pOwner, _float dt)
{
    _float fOnceRatio;
    _float fIntervalRatio;
    if (!m_pOwnerStateMachine->Get_Bool("Enhanced"))
    {
        fOnceRatio = 0.667f;
        fIntervalRatio = 0.375f;
    }
    else
    {
        fOnceRatio = 3.451f;
        fIntervalRatio = 3.451f;
    }

    for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "SawOnce")
        {
            pOwner->Begin_AttackCollider("Saw",
                HitDesc()
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * fOnceRatio * Helper::Get_Random_Float(1.0f, 1.5f)
                    , DAMAGE_TYPE::NORMAL)
                .Charge(0.f, 100.f)
            );
        }
        else if (Event.Tag == "SawInterval")
        {
            pOwner->Begin_AttackCollider("Saw",
                HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(pOwner->Get_AttackPower() * fIntervalRatio * Helper::Get_Random_Float(1.0f, 1.5f)
                    , DAMAGE_TYPE::HARD)
                .Interval(0.1f)
                .Charge(0.f, 50.f)
            );
        }
        else if (Event.Tag == "SawEnd")
        {
            pOwner->End_AttackCollider("Saw");
        }
        else if (Event.Tag == "PopInvincible")
        {
            pOwner->Pop_Invincible();
        }
    }

    if (!m_pSubStateMachine->Get_Bool("ExFinished"))
    {
        string strCurrentState = m_pSubStateMachine->Get_CurrentStateName();
        if (strCurrentState == "ExAttack_Loop" || strCurrentState == "ExAttack_Loop_Walk")
        {
            if (!InputDevice()->Key_Down('E'))
            {
                if (m_pSubStateMachine->Get_Bool("Enhanced"))
                    m_pSubStateMachine->Set_Int("ExplodeEntryMode", 2);
                else
                    m_pSubStateMachine->Set_Int("ExplodeEntryMode", 1);
                m_pSubStateMachine->Set_Trigger("ToExplode");
            }
            else if (m_pSubStateMachine->Get_Bool("Enhanced"))
            {
                if (pOwner->Get_EnergyDesc().fCurrentEnergy <= pOwner->Get_EnergyDesc().fSpecialEnergy)
                {
                    m_pSubStateMachine->Set_Int("ExplodeEntryMode", 2);
                    m_pSubStateMachine->Set_Trigger("ToExplode");
                }
            }
        }
    }

    auto pCorinState = pOwner->Get_StateMachine();
    if (pCorinState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentStateName() == "ExAttack_End" ||
            m_pSubStateMachine->Get_CurrentStateName() == "ExAttack_Explode" ||
            Is_AnimEnd())
        {
            pCorinState->Set_Trigger("SwitchOut");
            pCorinState->Set_Bool("OutReserve", false);
        }
    }

    __super::Update(pOwner, dt);
}

void CCorinState_ExAttack::Exit(CCorin* pOwner)
{
    pOwner->Reset_ReserveCombo();
    pOwner->Set_SpecialEnergy(80.f);
    pOwner->Get_StateMachine()->Set_Bool("Resistance", false);

    if (pOwner->Is_MainCharacter())
    {
        UI_ACTION_DESC desc;
        desc.eType = UI_ACTION_TYPE::SPECIAL;
        if (pOwner->Get_EnergyDesc().fCurrentEnergy >= pOwner->Get_EnergyDesc().fSpecialEnergy)
        {
            desc.eState = UI_ACTION_STATE::AVAILABLE;
        }
        else
        {
            desc.eState = UI_ACTION_STATE::ENABLE;
        }
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }

    __super::Exit(pOwner);
}

void CCorinState_ExAttack_Start::Enter(CCorin* pOwner)
{
    auto pAnimator = pOwner->Get_Animator();
    string strAnimName = pOwner->Get_Name();
    if (Get_ParentState()->Get_SubStateMachine()->Get_Bool("Enhanced"))
    {
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_02")
            .Speed(1.5f)
            .Apply();
        pOwner->Push_Invincible();
    }
    else
    {
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_01")
            .Speed(1.f)
            .Apply();
    }
}

void CCorinState_ExAttack_Start::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CCorinState_ExAttack_Start::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.15f))
        pOwner->Play_Effect("Corin_Saw_Slash0", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
    if (IsCrossAnimProgress(0.41f))
        pOwner->Play_Effect("Corin_Normal_Slash0", _vector3(-0.2f, 0.6f, 0.f), _quaternion(0.28f, -0.63f, 0.66f, 0.29f));
    if (IsCrossAnimProgress(0.52f))
        pOwner->Play_Effect("Corin_Normal_Slash1", _vector3(-0.1f, 0.7f, 0.f), _quaternion(0.6f, -0.37f, 0.39f, 0.59f));
    if (IsCrossAnimProgress(0.55f))
        pOwner->Play_Effect("Corin_Ex_Saw_Slash0", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
}

void CCorinState_ExAttack_Loop::Enter(CCorin* pOwner)
{
    if (Get_ParentState()->Get_SubStateMachine()->Get_Bool("Enhanced"))
    {
        pOwner->Unlock_Move();
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_03_Loop")
            .Loop(true)
            .Speed(1.f)
            .Apply();
    }
    else
    {
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_03_Loop")
            .Loop(false)
            .Speed(1.f)
            .Apply();
    }
}

void CCorinState_ExAttack_Loop::Update(CCorin* pOwner, _float dt)
{
    auto pSubStateMachine = Get_ParentState()->Get_SubStateMachine();
    _bool bEnhanced = pSubStateMachine->Get_Bool("Enhanced");
    if (pOwner->Is_Move_Buffer() && bEnhanced)
    {
        pSubStateMachine->Set_Trigger("ToWalk");
    }
    if (!bEnhanced && Is_AnimEnd())
    {
        pSubStateMachine->Set_Int("ExplodeEntryMode", 1);
        pSubStateMachine->Set_Trigger("ToExplode");
    }

    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    if(bEnhanced)
    {
        auto desc = pOwner->Get_EnergyDesc();
        pOwner->Set_CurrentEnergy(desc.fCurrentEnergy - desc.fEnergyWeight * dt);
    }
}

void CCorinState_ExAttack_Loop_Walk::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_03_Walk_Loop")
        .Loop(true)
        .Apply();
}

void CCorinState_ExAttack_Loop_Walk::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt);

    auto desc = pOwner->Get_EnergyDesc();
    pOwner->Set_CurrentEnergy(desc.fCurrentEnergy - desc.fEnergyWeight * dt);
}

void CCorinState_ExAttack_Loop_Walk::Exit(CCorin* pOwner)
{
    pOwner->Lock_Move();
}

void CCorinState_ExAttack_Explode::Enter(CCorin* pOwner)
{
    pOwner->End_AttackCollider("Saw");
    pOwner->Set_SpecialEnergy(80.f);
    auto pSubStateMachine = Get_ParentState()->Get_SubStateMachine();
    pSubStateMachine->Set_Bool("ExFinished", true);
    // 강화 상태일때 20소모
    if (pSubStateMachine->Get_Bool("Enhanced"))
    {
        pOwner->Set_CurrentEnergy(pOwner->Get_EnergyDesc().fCurrentEnergy - 20.f);
    }
    auto pAnimator = pOwner->Get_Animator();
    string strAnimName = pOwner->Get_Name();

    _int iEntryMode = pSubStateMachine->Get_Int("ExplodeEntryMode");
    pSubStateMachine->Set_Int("ExplodeEntryMode", 0);

    switch (iEntryMode)
    {
    case 1:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_01_Explode")
            .Loop(false)
            .Apply();
        break;
    case 2:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_02_Explode")
            .Loop(false)
            .Apply();
        break;
    }
    pSubStateMachine->Set_Int("EndEntryMode", iEntryMode);

    pOwner->Stop_Effect("Corin_Ex_Saw_Slash0");
}

void CCorinState_ExAttack_Explode::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_ExAttack_End::Enter(CCorin* pOwner)
{
    auto pAnimator = pOwner->Get_Animator();
    string strAnimName = pOwner->Get_Name();

    auto pSubStateMachine = Get_ParentState()->Get_SubStateMachine();
    _int iEntryMode = pSubStateMachine->Get_Int("EndEntryMode");
    pSubStateMachine->Set_Int("EndEntryMode", 0);

    switch (iEntryMode)
    {
    case 1:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_01_End")
            .Speed(1.5f)
            .Apply();
        break;
    case 2:
        pAnimator->Change_Animation(strAnimName + "Attack_Branch_02_End")
            .Speed(1.5f)
            .Apply();
        break;
    }

    pOwner->Stop_Effect("Corin_Saw_Slash0");
}