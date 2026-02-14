#include "pch.h"
#include "MiyabiState_ChargeAttack.h"
#include "MiyabiState_Attack.h"
#include "Miyabi.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "Animator3D.h"
#include "AudioSource.h"
#include "EffectContainer.h"

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

    auto pMiyabiState = pOwner->Get_StateMachine();
    if (pMiyabiState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentStateName() == "End" ||
            Is_AnimEnd())
        {
            pMiyabiState->Set_Trigger("SwitchOut");
            pMiyabiState->Set_Bool("OutReserve", false);
        }
    }

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
    pOwner->Hide_Ghost();
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
    if(IsCrossAnimProgress(0.16f))
        pOwner->Play_Effect("Miyabi_Charge_Start", _vector3(0.f, 0.1f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);

}

void CMiyabiState_Charge_Start::Exit(CMiyabi* pOwner)
{
    m_pOwnerStateMachine->Set_Int("ChargeLevel", 1);
}

void CMiyabiState_Charge_Start::Update_Effects(CMiyabi* pOwner)
{
}

// Charge_Start_02 (2단)
void CMiyabiState_Charge_Start_02::Enter(CMiyabi* pOwner)
{
    pOwner->Decrease_Frost(2);
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Start_02")
        .Speed(1.3f)
        .Apply();
    pOwner->Get_Component<CAudioSource>()->Slot("Miyabi_ChargeStart02_Voice")
        .Attribute3D(true)
        .Play();
    pOwner->Set_WeaponEffectMesh(true);
    pOwner->Play_Effect("Miyabi_Charge_StackUp0", _vector3(0.f, 1.f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
}

void CMiyabiState_Charge_Start_02::Update(CMiyabi* pOwner, _float dt)
{
    Update_Effects(pOwner);
}

void CMiyabiState_Charge_Start_02::Exit(CMiyabi* pOwner)
{
    m_pOwnerStateMachine->Set_Int("ChargeLevel", 2);
}

void CMiyabiState_Charge_Start_02::Update_Effects(CMiyabi* pOwner)
{
       
}

// Charge_Start_03 (3단)
void CMiyabiState_Charge_Start_03::Enter(CMiyabi* pOwner)
{
    pOwner->Decrease_Frost(2);
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ChargeAttack_Start_03")
        .Speed(1.5f)
        .Apply();

    pOwner->Play_Effect("Miyabi_Charge_StackUp1", _vector3(0.f, 1.f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
}

void CMiyabiState_Charge_Start_03::Update(CMiyabi* pOwner, _float dt)
{
    Update_Effects(pOwner);
}

void CMiyabiState_Charge_Start_03::Exit(CMiyabi* pOwner)
{
    m_pOwnerStateMachine->Set_Int("ChargeLevel", 3);
}

void CMiyabiState_Charge_Start_03::Update_Effects(CMiyabi* pOwner)
{
}

// Charge_End
void CMiyabiState_Charge_End::Enter(CMiyabi* pOwner)
{
    pOwner->Show_Ghost();
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
    _vector3 vPos = pOwner->Get_WorldPos();
    _vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
    if (IsCrossAnimProgress(0.21f))
    {
        if (iLevel == 1)
        {
            BattleSystem()->TakeAreaDamage(vPos, 8.f, vLook, 30.f, HitDesc()
                .Name(pOwner->Get_CharacterName())
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
        .Speed(2.5f)
        .ReserveSpeed(0.f, 0.33f, 1.5f, EaseType::InExpo)
        .ReserveSpeed(0.33f, 0.88f, 1.f, EaseType::InOutBack)
        .ReserveSpeed(0.88f, 0.9f, 0.5f, EaseType::OutExpo)
        .ReserveSpeed(0.9f, 1.f, 1.5f, EaseType::OutQuart)
        .Apply();
    pOwner->Get_Component<CAudioSource>()->Slot("Miyabi_ChargeAttack03_Voice")
        .Attribute3D(true)
        .Play();

    m_iRepeatCount = 0;
    m_iStingRepeatCount = 0;
    m_fRepeatProgress = 0.07f;
    m_fStingRepeatProgress = 0.1f;
    m_OnEndAttack = false;
}

void CMiyabiState_Charge_Attack03::Update(CMiyabi* pOwner, _float dt)
{
    CCharacter::ROOTMOTION_DESC desc;
    desc.fMoveWeight = 1.f;// m_fAnimProgress < 0.2f ? 1.5f : 1.8f;
    desc.iModeMask = ENUM(CMiyabi::ROOTMOTION_MASK::MOVE)
        | ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION);
    pOwner->Process_RootMotion(dt, desc);

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
                .Name(pOwner->Get_CharacterName())
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 1.07055f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::NORMAL)
            );

        }
    }

    if (IsCrossAnimProgress(0.9f))
        pOwner->Get_Component<CAudioSource>()->Sequence("ChargeAttack03")
        .Attribute3D(true)
        .PlayNext();

    Update_Effects(pOwner);
}

void CMiyabiState_Charge_Attack03::Exit(CMiyabi* pOwner)
{
    m_bAreaAttack = false;
}

void CMiyabiState_Charge_Attack03::Update_Effects(CMiyabi* pOwner)
{
    if (m_fAnimProgress < 0.33f)
    {
        if (IsCrossAnimProgress(m_fRepeatProgress))
        {
            _vector3 vRandPosition{}, vRandAngle{};
            vRandPosition.x = Helper::Get_Random_Float(m_vMinRange.x, m_vMaxRange.x);
            //vRandPosition.y = Helper::Get_Random_Float(m_vMinRange.y, m_vMaxRange.y);
            vRandPosition.z = Helper::Get_Random_Float(m_vMinRange.z, m_vMaxRange.z);

            vRandAngle.x = XMConvertToRadians(Helper::Get_Random_Float(-60.f, 60.f));
            vRandAngle.y = XMConvertToRadians(Helper::Get_Random_Float(-100.f, 100.f));

            _quaternion vRandRotation = _quaternion::CreateFromYawPitchRoll(vRandAngle);
            pOwner->Play_Effect("Miyabi_Charge0_Slash" + to_string(m_iRepeatCount % 15), vRandPosition, vRandRotation, false);

            m_fRepeatProgress += m_fRepeatInterval;
            ++m_iRepeatCount;
        }

        if (IsCrossAnimProgress(m_fStingRepeatProgress))
        {
            _float3 vRandPosition{}, vRandRotation{}, vCenter{};
            vCenter.x = 0.f;
            vCenter.y = 2.f;
            vCenter.z = 1.f;

            vRandPosition.x = Helper::Get_Random_Float(m_vStingMinRange.x, m_vStingMaxRange.x);
            vRandPosition.y = Helper::Get_Random_Float(m_vStingMinRange.y, m_vStingMaxRange.y);
            vRandPosition.z = Helper::Get_Random_Float(m_vStingMinRange.z, m_vStingMaxRange.z);

            vRandRotation.x = 0.f;
            vRandRotation.y = XMConvertToRadians(Helper::Get_Random_Float(-5.f, 5.f));
            vRandRotation.z = XMConvertToRadians(Helper::Get_Random_Float(-10.f, 10.f));


            _vector3 vDir = vCenter - vRandPosition;
            vDir.Normalize();

            _float yaw = atan2(vDir.z, vDir.x);
            _float roll = atan2(vDir.y, sqrtf(vDir.x * vDir.x + vDir.z * vDir.z));

            _quaternion rotation = _quaternion::CreateFromYawPitchRoll(yaw, 0.f, roll);
            rotation *= _quaternion::CreateFromYawPitchRoll(vRandRotation);

            pOwner->Play_Effect("Miyabi_Ex1_Sting" + to_string(m_iStingRepeatCount % 9), _vector3(vRandPosition), rotation, false);

            m_fStingRepeatProgress += m_fStingRepeatInterval;
            ++m_iStingRepeatCount;
        }
    }

    if (IsCrossAnimProgress(0.88f))
    {
        pOwner->Play_Effect("Miyabi_Charge0_Flare1", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
        pOwner->Play_Effect("Miyabi_Charge0_Particle1", _vector3(0.f, 0.f, 3.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
        pOwner->Play_Effect("Miyabi_Charge0_Smoke", _vector3(0.f, 0.f, 3.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);

        m_fRepeatProgress = 0.88f;
        m_fStingRepeatProgress = 0.88f;
        m_OnEndAttack = true;
    }

    if (m_OnEndAttack)
    {
        if (IsCrossAnimProgress(m_fRepeatProgress))
        {
            _vector3 vRandPosition{}, vRandAngle{};
            vRandPosition.x = Helper::Get_Random_Float(m_vMinRange.x, m_vMaxRange.x);
            //vRandPosition.y = Helper::Get_Random_Float(m_vMinRange.y, m_vMaxRange.y);
            vRandPosition.z = Helper::Get_Random_Float(m_vMinRange.z, m_vMaxRange.z);

            vRandAngle.x = XMConvertToRadians(Helper::Get_Random_Float(-60.f, 60.f));
            vRandAngle.y = XMConvertToRadians(Helper::Get_Random_Float(-100.f, 100.f));

            _quaternion vRandRotation = _quaternion::CreateFromYawPitchRoll(vRandAngle);
            pOwner->Play_Effect("Miyabi_Charge0_Slash" + to_string(m_iRepeatCount % 15), vRandPosition, vRandRotation, false);

            m_fRepeatProgress += m_fRepeatInterval;
            ++m_iRepeatCount;
        }

        if (IsCrossAnimProgress(m_fStingRepeatProgress))
        {
            _float3 vRandPosition{}, vRandRotation{}, vCenter{};
            vCenter.x = 0.f;
            vCenter.y = 2.f;
            vCenter.z = 1.f;

            vRandPosition.x = Helper::Get_Random_Float(m_vStingMinRange.x, m_vStingMaxRange.x);
            vRandPosition.y = Helper::Get_Random_Float(m_vStingMinRange.y, m_vStingMaxRange.y);
            vRandPosition.z = Helper::Get_Random_Float(m_vStingMinRange.z, m_vStingMaxRange.z);

            vRandRotation.x = 0.f;
            vRandRotation.y = XMConvertToRadians(Helper::Get_Random_Float(-5.f, 5.f));
            vRandRotation.z = XMConvertToRadians(Helper::Get_Random_Float(-10.f, 10.f));


            _vector3 vDir = vCenter - vRandPosition;
            vDir.Normalize();

            _float yaw = atan2(vDir.z, vDir.x);
            _float roll = atan2(vDir.y, sqrtf(vDir.x * vDir.x + vDir.z * vDir.z));

            _quaternion rotation = _quaternion::CreateFromYawPitchRoll(yaw, 0.f, roll);
            rotation *= _quaternion::CreateFromYawPitchRoll(vRandRotation);

            pOwner->Play_Effect("Miyabi_Ex1_Sting" + to_string(m_iStingRepeatCount % 9), _vector3(vRandPosition), rotation, false);

            m_fStingRepeatProgress += m_fStingRepeatInterval;
            ++m_iStingRepeatCount;
        }
    }
}

