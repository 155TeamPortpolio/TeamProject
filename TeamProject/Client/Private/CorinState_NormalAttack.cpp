#include "pch.h"
#include "CorinState_NormalAttack.h"
#include "Corin.h"
#include "GameInstance.h"
#include "Animator3D.h"

CCorinState_NormalAttack* CCorinState_NormalAttack::Create()
{
    auto pInstance = new CCorinState_NormalAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CCorin>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Attack_01", CCorinState_Attack_01::Create());
    pSubStateMachine->Register_State("Attack_02", CCorinState_Attack_02::Create());
    pSubStateMachine->Register_State("Attack_03", CCorinState_Attack_03::Create());
    pSubStateMachine->Register_State("Attack_04", CCorinState_Attack_04::Create());
    pSubStateMachine->Register_State("Attack_05", CCorinState_Attack_05::Create());
    pSubStateMachine->Register_State("Attack_End", CCorinState_Attack_End::Create());

    pSubStateMachine->Get_State("Attack_End")->Set_Tag("End");

    // 콤보 전이: Trigger + AnimEnd : 애니매이션중 마우스가 눌렸고 애니매이션이 끝나면 다음 재생
    vector<CStateMachine<CCorin>::CONDITION_INFO> comboConditions;
    comboConditions.push_back({ CStateMachine<CCorin>::CONDITION_TRIGGER, "NextCombo", 0.f });
    comboConditions.push_back({ CStateMachine<CCorin>::CONDITION_ANIMATION_END, "", 0.f });

    pSubStateMachine->Register_Transition("Attack_01", "Attack_02", comboConditions);
    pSubStateMachine->Register_Transition("Attack_02", "Attack_03", comboConditions);
    pSubStateMachine->Register_Transition("Attack_03", "Attack_04", comboConditions);
    pSubStateMachine->Register_Transition("Attack_04", "Attack_05", comboConditions);

    // End 전이
    pSubStateMachine->Register_Transition("Attack_01", "Attack_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Attack_02", "Attack_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Attack_03", "Attack_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Attack_04", "Attack_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Attack_05", "Attack_End",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("Attack_01");

    return pInstance;
}

void CCorinState_NormalAttack::Enter(CCorin* pOwner)
{
    pOwner->Lock_Move();
    m_iComboIndex = 0;

    // 부모 상태머신에서 진입 인덱스 가져오기
    _int iEntryIndex = m_pParentState->Get_SubStateMachine()->Get_Int("ComboEntryIndex");
    m_pParentState->Get_SubStateMachine()->Set_Int("ComboEntryIndex", 0);

    m_iComboIndex = iEntryIndex;
    const string arrEntryStates[5] = {
        "Attack_01", "Attack_02", "Attack_03", "Attack_04", "Attack_05"
    };
    m_pSubStateMachine->Set_DefaultState(arrEntryStates[iEntryIndex]);

    m_pSubStateMachine->Reset_Trigger("NextCombo");
    __super::Enter(pOwner);
}

void CCorinState_NormalAttack::Update(CCorin* pOwner, _float dt)
{
    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Tap(MOUSE_BTN::LB))
        m_pSubStateMachine->Set_Trigger("NextCombo");

    auto pCorinState = pOwner->Get_StateMachine();
    if (pCorinState->Get_Bool("OutReserve"))
    {
        if(m_pSubStateMachine->Get_CurrentStateName() == "Attack_End" ||
            Is_AnimEnd())
        {
            pCorinState->Set_Trigger("SwitchOut");
            pCorinState->Set_Bool("OutReserve", false);
        }
    }

    __super::Update(pOwner, dt);
}

void CCorinState_NormalAttack::Exit(CCorin* pOwner)
{
    pOwner->Reset_ReserveCombo();
    __super::Exit(pOwner);
}


#pragma region SubStates
void CCorinState_Attack_01::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_01")
        .Speed(1.5f)
        .ReserveSpeed(0.37f, 0.45f, 0.4f, EaseType::OutQuart)
        .ReserveSpeed(0.45f, 0.5f, 1.5f, EaseType::Linear)
        .Apply();
}

void CCorinState_Attack_01::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "SawCount")
        {
            pOwner->Begin_AttackCollider("Saw",
                HitDesc()
                .Type(HIT_TYPE::COUNT)
                .Damage(pOwner->Get_AttackPower() * 0.82f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::NORMAL)
                .Interval(0.05f)
                .MaxCount(4)
                .Charge(1.f, 10.f)
            );
        }
        else if (Event.Tag == "SawEnd")
        {
            pOwner->End_AttackCollider("Saw");
        }
    }

    Update_Effects(pOwner);
}

void CCorinState_Attack_01::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(0);
}

void CCorinState_Attack_01::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.4f))
        pOwner->Play_Effect("Corin_Normal_Slash0", _vector3(0.2f, 0.5f, 0.2f), _quaternion(0.75f, -0.11f, 0.05f, -0.65f));
}

void CCorinState_Attack_02::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_02")
        .Speed(1.5f)
        .ReserveSpeed(0.25f, 0.35f, 0.4f, EaseType::OutQuart)
        .ReserveSpeed(0.35f, 0.40f, 1.5f, EaseType::Linear)
        .Apply();
}

void CCorinState_Attack_02::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "SawCount")
        {
            pOwner->Begin_AttackCollider("Saw",
                HitDesc()
                .Type(HIT_TYPE::COUNT)
                .Damage(pOwner->Get_AttackPower() * 0.766f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::NORMAL)
                .Interval(0.05f)
                .MaxCount(4)
                .Charge(1.f, 10.f)
            );
        }
        else if (Event.Tag == "SawEnd")
        {
            pOwner->End_AttackCollider("Saw");
        }
    }

    Update_Effects(pOwner);
}

void CCorinState_Attack_02::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(1);
}

void CCorinState_Attack_02::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.27f))
        pOwner->Play_Effect("Corin_Normal_Slash0", _vector3(0.f, 1.f, 0.f), _quaternion(0.64f, 0.f, 0.74f, 0.19f));
}

void CCorinState_Attack_03::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_03")
        .Speed(1.5f)
        .ReserveSpeed(0.5f, 1.f, 0.55f, EaseType::OutQuart)
        .Apply();
}

void CCorinState_Attack_03::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "SawInterval")
        {
            pOwner->Begin_AttackCollider("Saw",
                HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(pOwner->Get_AttackPower() * 1.792f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::NORMAL)
                .Interval(0.05f)
                .Charge(1.f, 10.f)
            );
        }
        else if (Event.Tag == "SawEnd")
        {
            pOwner->End_AttackCollider("Saw");
        }
    }

    Update_Effects(pOwner);
}

void CCorinState_Attack_03::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(2);
}

void CCorinState_Attack_03::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.41f))
        pOwner->Play_Effect("Corin_Sting0", _vector3(0.3f, 0.7f, 0.7f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
}

void CCorinState_Attack_04::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_04")
        .Speed(1.5f)
        .ReserveSpeed(0.22f, 0.46f, 0.55f, EaseType::OutQuart)
        .ReserveSpeed(0.46f, 0.51f, 1.5f, EaseType::Linear)
        .Apply();
}

void CCorinState_Attack_04::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "SawInterval")
        {
            pOwner->Begin_AttackCollider("Saw",
                HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(pOwner->Get_AttackPower() * 2.334f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::NORMAL)
                .Interval(0.05f)
                .Charge(1.f, 10.f)
            );
        }
        else if (Event.Tag == "SawEnd")
        {
            pOwner->End_AttackCollider("Saw");
        }
    }

    Update_Effects(pOwner);
}

void CCorinState_Attack_04::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(3);
}

void CCorinState_Attack_04::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.16f))
        pOwner->Play_Effect("Corin_Normal_Slash0", _vector3(-0.1f, 0.5f, 0.1f), _quaternion(0.13f, 0.7f, 0.69f, -0.09f));

    if (IsCrossAnimProgress(0.46f))
        pOwner->Play_Effect("Corin_Normal_Slash1", _vector3(0.1f, 1.1f, -0.1f), _quaternion(0.72f, 0.f, -0.63f, -0.29f));
}

void CCorinState_Attack_05::Enter(CCorin* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_05")
        .Speed(1.5f)
        .ReserveSpeed(0.4f, 1.f, 0.5f, EaseType::OutQuart)
        .Apply();
}

void CCorinState_Attack_05::Update(CCorin* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
        ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));

    for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "SawInterval")
        {
            pOwner->Begin_AttackCollider("Saw",
                HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(pOwner->Get_AttackPower() * 4.212f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::HARD)
                .Interval(0.05f)
                .Charge(1.f, 10.f)
            );
        }
        else if (Event.Tag == "SawEnd")
        {
            pOwner->End_AttackCollider("Saw");
        }
    }

    Update_Effects(pOwner);
}

void CCorinState_Attack_05::Exit(CCorin* pOwner)
{
    static_cast<CCorinState_NormalAttack*>(m_pParentState)->Set_ComboIndex(4);
}

void CCorinState_Attack_05::Update_Effects(CCorin* pOwner)
{
    if (IsCrossAnimProgress(0.37f))
        pOwner->Play_Effect("Corin_Normal2_Slash0", _vector3(0.f, 0.2f, -0.5f), _quaternion(0.07f, -0.63f, 0.22f, 0.74f));
}

void CCorinState_Attack_End::Enter(CCorin* pOwner)
{
    CCorinState_NormalAttack* pParent = static_cast<CCorinState_NormalAttack*>(m_pParentState);
    _uint iIndex = pParent ? pParent->Get_ComboIndex() : 0;

    const string arrEndAnims[5] =
    {
        pOwner->Get_Name() + "Attack_Normal_01_End",
        pOwner->Get_Name() + "Attack_Normal_02_End",
        pOwner->Get_Name() + "Attack_Normal_03_End",
        pOwner->Get_Name() + "Attack_Normal_04_End",
        pOwner->Get_Name() + "Attack_Normal_05_End"
    };

    pOwner->Get_Animator()->Change_Animation(arrEndAnims[iIndex])
        .Speed(1.2f)
        .Apply();

    pOwner->Unlock_Move();
}
#pragma endregion


