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
    pOwner->Set_WeaponEffectMesh(true);
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
    pOwner->Set_ResetMove(true);
    __super::Exit(pOwner);
}

void CMiyabiState_SwitchInAttack_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "SwitchIn_Attack")
        .Loop(false)
        .Apply();

    m_vPos = pOwner->Get_WorldPos();
    m_vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);

    m_iRepeatCount = 0;
    m_fRepeatProgress = 0.53f;
    m_vStartRotation = _float4(0.f, 0.f, 0.f, 1.f);

    m_iStingRepeatCount = 0;
    m_fStingRepeatProgress = 0.53f;
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
                    , DAMAGE_TYPE::NORMAL)
            );
        }
        else if (Event.Tag == "AreaAttackHard")
        {
            BattleSystem()->TakeAreaDamage(m_vPos + m_vLook * 7.f, 7.f, HitDesc()
                .Name(pOwner->Get_CharacterName())
                .Type(HIT_TYPE::ONCE)
                .Damage(pOwner->Get_AttackPower() * 0.897f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::HARD)
            );
        }
    }

    Update_Effects(pOwner);
}

void CMiyabiState_SwitchInAttack_Start::Update_Effects(CMiyabi* pOwner)
{
    //1
    if (IsCrossAnimProgress(0.19f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash0", _vector3(-6.6f, 0.7f, 5.1f), _quaternion(-0.27f, 0.91f, 0.11f, -0.31f), false);
        pOwner->Play_Effect("Miyabi_Ultimate1_Sting0", _vector3(-2.5f, 0.5f, 2.2f), _quaternion(0.f, 0.94f, 0.f, -0.34f), false);
    }
    if (IsCrossAnimProgress(0.21f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash1", _vector3(-6.6f, 0.7f, 5.1f), _quaternion(-0.37f, -0.02f, -0.22f, 0.9f), false);
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash2", _vector3(-6.6f, 0.7f, 5.1f), _quaternion(0.14f, -0.5f, 0.84f, 0.17f), false);
    }

    //2
    if(IsCrossAnimProgress(0.26f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash3", _vector3(6.5f, 0.7f, 9.1f), _quaternion(-0.44f, 0.73f, 0.25f, -0.46f), false);
        pOwner->Play_Effect("Miyabi_Ultimate0_Sting0", _vector3(-6.2f, 0.2f, 5.3f), _quaternion(0.f, -0.18f, 0.f, 0.98f), false);
    }
    if(IsCrossAnimProgress(0.28f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash4", _vector3(6.5f, 0.7f, 9.1f), _quaternion(0.71f, -0.09f, 0.12f, -0.69f), false);
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash5", _vector3(6.5f, 0.7f, 9.1f), _quaternion(-0.4f, 0.6f, -0.65f, -0.25f), false);
    }

    //3
    if (IsCrossAnimProgress(0.33f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash0", _vector3(0.f, 0.7f, 14.1f), _quaternion(-0.44f, 0.73f, 0.25f, -0.46f), false);
        pOwner->Play_Effect("Miyabi_Ultimate1_Sting0", _vector3(4.8f, 0.2f, 10.8f), _quaternion(0.f, 0.94f, 0.f, -0.35f), false);
    }
    if (IsCrossAnimProgress(0.35f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash1", _vector3(0.f, 0.7f, 14.1f), _quaternion(0.71f, -0.09f, 0.12f, -0.69f), false);
        pOwner->Play_Effect("Miyabi_Ultimate0_Slash2", _vector3(0.f, 0.7f, 14.1f), _quaternion(-0.4f, 0.6f, -0.65f, -0.25f), false);
    }
    
    // Return
    if (IsCrossAnimProgress(0.53f))
    {
        pOwner->Play_Effect("Miyabi_Ultimate_Smoke0", _vector3(0.f, 0.1f, 14.1f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
        pOwner->Play_Effect("Miyabi_Ultimate_Smoke1", _vector3(0.f, 0.1f, 14.1f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
    }

    if (m_iRepeatCount < 30)
    {
        if (IsCrossAnimProgress(m_fRepeatProgress))
        {
            _quaternion deltaRotation = _quaternion::CreateFromYawPitchRoll(XMConvertToRadians(-45.f), XMConvertToRadians(60.f), XMConvertToRadians(45.f));
            _quaternion currRotation = m_vStartRotation;
            currRotation *= deltaRotation;

            _vector3 randAngle{};
            randAngle.x = Helper::Get_Random_Float(-5.f, 5.f);
            randAngle.y = Helper::Get_Random_Float(-5.f, 5.f);
            randAngle.z = Helper::Get_Random_Float(-5.f, 5.f);
            _quaternion randRotation = _quaternion::CreateFromYawPitchRoll(randAngle);
            currRotation *= randRotation;
            currRotation.Normalize();

            m_vStartRotation = currRotation;

            pOwner->Play_Effect("Miyabi_Ex1_Slash" + to_string(m_iRepeatCount % 9), _vector3(0.f, 0.5f, 14.f - (m_iRepeatCount * m_fDistanceInterval)), currRotation, false);

            m_fRepeatProgress += m_fRepeatInterval;
            ++m_iRepeatCount;
        }
    }

    if (m_iStingRepeatCount < 10)
    {
        if (IsCrossAnimProgress(m_fStingRepeatProgress))
        {
            _float3 vRandPosition{}, vRandRotation{}, vCenter{};
            vCenter.x = 0.f;
            vCenter.y = 2.f;
            vCenter.z = 13.6f - (m_iRepeatCount * m_fDistanceInterval);

            vRandPosition.x = Helper::Get_Random_Float(m_vMinRange.x, m_vMaxRange.x);
            vRandPosition.y = Helper::Get_Random_Float(m_vMinRange.y, m_vMaxRange.y);
            vRandPosition.z = vCenter.z + Helper::Get_Random_Float(m_vMinRange.z, m_vMaxRange.z);

            vRandRotation.x = 0.f;
            vRandRotation.y = XMConvertToRadians(Helper::Get_Random_Float(-5.f, 5.f));
            vRandRotation.z = XMConvertToRadians(Helper::Get_Random_Float(-10.f, 10.f));


            _vector3 vDir = vCenter - vRandPosition;
            vDir.Normalize();

            _float yaw = atan2(vDir.z, vDir.x);
            _float roll = atan2(vDir.y, sqrtf(vDir.x * vDir.x + vDir.z * vDir.z));

            _quaternion rotation = _quaternion::CreateFromYawPitchRoll(yaw, 0.f, roll);
            rotation *= _quaternion::CreateFromYawPitchRoll(vRandRotation);

            pOwner->Play_Effect("Miyabi_Ex1_Sting" + to_string(m_iRepeatCount % 9), _vector3(vRandPosition), rotation, false);

            m_fStingRepeatProgress += m_fStingRepeatInterval;
            ++m_iStingRepeatCount;
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
