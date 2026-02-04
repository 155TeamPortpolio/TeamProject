#include "pch.h"
#include "MiyabiState_ChargeAttack.h"
#include "MiyabiState_Attack.h"
#include "Miyabi.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "Animator3D.h"

CMiyabiState_ChargeAttack* CMiyabiState_ChargeAttack::Create()
{
    auto pInstance = new CMiyabiState_ChargeAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("Charge_Start", CMiyabiState_Charge_Start::Create());
    pSubStateMachine->Register_State("Charge_Start_02", CMiyabiState_Charge_Start_02::Create());
    pSubStateMachine->Register_State("Charge_Start_03", CMiyabiState_Charge_Start_03::Create());
    pSubStateMachine->Register_State("Charge_Attack01", CMiyabiState_Charge_Attack01::Create());
    pSubStateMachine->Register_State("Charge_Attack03", CMiyabiState_Charge_Attack03::Create());
    pSubStateMachine->Register_State("Charge_End", CMiyabiState_Charge_End::Create());

    pSubStateMachine->Get_State("Charge_End")->Set_Tag("End");

    // 충전 단계 전이: AnimEnd & 게이지 있을때 다음 단계
    vector<CStateMachine<CMiyabi>::CONDITION_INFO> Conditions;
    Conditions.push_back({ CStateMachine<CMiyabi>::CONDITION_ANIMATION_END });
    Conditions.push_back({ CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "CanCharge"});
    pSubStateMachine->Register_Transition("Charge_Start", "Charge_Start_02",
        Conditions);
    pSubStateMachine->Register_Transition("Charge_Start_02", "Charge_Start_03",
        Conditions);
    pSubStateMachine->Register_Transition("Charge_Start", "Charge_Attack01",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Charge_Start_02", "Charge_Attack01",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    // 3단 충전 완료 시 자동 공격
    pSubStateMachine->Register_Transition("Charge_Start_03", "Charge_Attack03",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    // 버튼 놓으면 공격 (Release 트리거)
    pSubStateMachine->Register_Transition("Charge_Start", "Charge_Attack01",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Release");
    pSubStateMachine->Register_Transition("Charge_Start_02", "Charge_Attack01",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Release");
    pSubStateMachine->Register_Transition("Charge_Start_03", "Charge_Attack03",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Release");

    // 공격 → End
    pSubStateMachine->Register_Transition("Charge_Attack01", "Charge_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("Charge_Attack03", "Charge_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("Charge_Start");

    return pInstance;
}

void CMiyabiState_ChargeAttack::Enter(CMiyabi* pOwner)
{
    m_pSubStateMachine->Set_Int("ChargeLevel", 0);
    m_pSubStateMachine->Reset_Trigger("Release");

    pOwner->Push_Invincible();

    __super::Enter(pOwner);
}

void CMiyabiState_ChargeAttack::Update(CMiyabi* pOwner, _float dt)
{
    // 버튼 놓으면 Release 트리거
    if (InputDevice()->Mouse_Away(MOUSE_BTN::LB))
        m_pSubStateMachine->Set_Trigger("Release");

    m_pSubStateMachine->Set_Bool("CanCharge", pOwner->Can_Charge());

    __super::Update(pOwner, dt);
}

void CMiyabiState_ChargeAttack::Exit(CMiyabi* pOwner)
{
    pOwner->Pop_Invincible();
    __super::Exit(pOwner);
}

// ===================== Sub States =====================

// Charge_Start (1단)
void CMiyabiState_Charge_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Decrease_Frost(2);
    CMiyabiState_ChargeAttack* pChargeAttack = static_cast<CMiyabiState_ChargeAttack*>(m_pParentState);
    CMiyabiState_Attack* pAttack = pChargeAttack ? static_cast<CMiyabiState_Attack*>(pChargeAttack->Get_ParentState()) : nullptr;

    _bool bFromCombo = false;
    if (pAttack && pAttack->Get_SubStateMachine())
        bFromCombo = (pAttack->Get_SubStateMachine()->Get_PrevStateName() == "NormalAttack");

    if (bFromCombo)
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Start_Front")
        .Apply();
    else
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Start")
        .Apply();
}
void CMiyabiState_Charge_Start::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Charge_Start::Exit(CMiyabi* pOwner)
{
    m_pOwnerStateMachine->Set_Int("ChargeLevel", 1);
}

// Charge_Start_02 (2단)
void CMiyabiState_Charge_Start_02::Enter(CMiyabi* pOwner)
{
    pOwner->Decrease_Frost(2);
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Start_02")
        .Apply();
}

void CMiyabiState_Charge_Start_02::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Charge_Start_02::Exit(CMiyabi* pOwner)
{
    m_pOwnerStateMachine->Set_Int("ChargeLevel", 2);
}

// Charge_Start_03 (3단)
void CMiyabiState_Charge_Start_03::Enter(CMiyabi* pOwner)
{
    pOwner->Decrease_Frost(2);
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Start_03")
        .Apply();
}

void CMiyabiState_Charge_Start_03::Update(CMiyabi* pOwner, _float dt)
{
}

void CMiyabiState_Charge_Start_03::Exit(CMiyabi* pOwner)
{
    m_pOwnerStateMachine->Set_Int("ChargeLevel", 3);
}

// Charge_End
void CMiyabiState_Charge_End::Enter(CMiyabi* pOwner)
{
    _uint iLevel = m_pOwnerStateMachine->Get_Int("ChargeLevel");

    if (iLevel >= 3)
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Attack03_End")
        .Apply();
    else
        pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Attack01_End")
        .Apply();
}

// Charge_Attack01 (1~2단 공격)
void CMiyabiState_Charge_Attack01::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Attack01")
        .Apply();
}

void CMiyabiState_Charge_Attack01::Update(CMiyabi* pOwner, _float dt)
{
    _uint iLevel = m_pOwnerStateMachine->Get_Int("ChargeLevel");
    _vector3 vPos = pOwner->Get_Component<CTransform>()->Dir(STATE::POSITION);
    _vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
    if (IsCrossAnimProgress(0.21f))
    {
        if (iLevel == 1)
        {
            BattleSystem()->TakeAreaDamage(vPos, 8.f, vLook, 30.f, HitDesc()
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 4.547f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::NORMAL)
            );
        }
        else
        {
            BattleSystem()->TakeAreaDamage(vPos, 8.f, vLook, 60.f, HitDesc()
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 8.581f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::NORMAL)
            );
        }
    }
}

// Charge_Attack03 (3단 공격)
void CMiyabiState_Charge_Attack03::Enter(CMiyabi* pOwner)
{
    m_bAreaAttack = false;
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Attack03")
        .Apply();
}

void CMiyabiState_Charge_Attack03::Update(CMiyabi* pOwner, _float dt)
{
    for (const auto& Event : pOwner->Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "AreaAttackStart")
        {
            m_bAreaAttack = true;
            m_fAreaTimer = 0.f;
        }
        else if (Event.Tag == "AreaAttackEnd")
        {
            m_bAreaAttack = false;
        }
    }

    if (m_bAreaAttack)
    {
        m_fAreaTimer += dt;
        if (m_fAreaTimer >= m_fAreaInterval)
        {
            m_fAreaTimer -= m_fAreaInterval;
            _vector3 vPos = pOwner->Get_WorldPos();
            _vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
            BattleSystem()->TakeAreaDamage(vPos, 8.f, HitDesc()
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 1.07055f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::NORMAL)
            );

        }
    }
}

void CMiyabiState_Charge_Attack03::Exit(CMiyabi* pOwner)
{
    m_bAreaAttack = false;
}

