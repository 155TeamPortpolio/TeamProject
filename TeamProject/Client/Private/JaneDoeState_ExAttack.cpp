#include "pch.h"
#include "JaneDoeState_ExAttack.h"
#include "GameInstance.h"

#include "JaneDoe.h"

CJaneDoeState_ExAttack* CJaneDoeState_ExAttack::Create()
{
    auto pInstance = new CJaneDoeState_ExAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("ExAttack_Start", CJaneDoeState_ExAttack_Start::Create());
    pSubStateMachine->Register_State("ExAttack_End", CJaneDoeState_ExAttack_End::Create());

    pSubStateMachine->Get_State("ExAttack_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("ExAttack_Start", "ExAttack_End",
        CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("ExAttack_Start");

    return pInstance;
}

void CJaneDoeState_ExAttack::Enter(CJaneDoe* pOwner)
{
    CCharacter::EnergyDesc energyDesc = pOwner->Get_EnergyDesc();
    energyDesc.fCurrentEnergy -= energyDesc.fSpecialEnergy;
    pOwner->Set_CurrentEnergy(energyDesc.fCurrentEnergy);

    UI_ACTION_DESC desc;
    desc.eType = UI_ACTION_TYPE::SPECIAL;
    if (energyDesc.fCurrentEnergy >= energyDesc.fSpecialEnergy)
    {
        desc.eState = UI_ACTION_STATE::AVAILABLE;
    }
    else
    {
        desc.eState = UI_ACTION_STATE::EXECUTING;
    }
    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });


    pOwner->Push_Invincible();
    __super::Enter(pOwner);
}

void CJaneDoeState_ExAttack::Update(CJaneDoe* pOwner, _float dt)
{
    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

        if (Event.Tag == "LFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_L", HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(pOwner->Get_AttackPower() * 5.747f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::HARD)
                .Interval(0.05f)
            );
        }
        else if (Event.Tag == "LFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_L");
        }
        else if (Event.Tag == "RFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_R", HitDesc()
                .Type(HIT_TYPE::INTERVAL)
                .Damage(pOwner->Get_AttackPower() * 5.747f * Helper::Get_Random_Float(1.f, 1.5f)
                    , DAMAGE_TYPE::HARD)
                .Interval(0.05f)
            );
        }
        else if (Event.Tag == "RFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_R");
        }
    }

    auto pJaneDoeState = pOwner->Get_StateMachine();
    if (pJaneDoeState->Get_Bool("OutReserve"))
    {
        if (m_pSubStateMachine->Get_CurrentStateName() == "End" ||
            Is_AnimEnd())
        {
            pJaneDoeState->Set_Trigger("SwitchOut");
            pJaneDoeState->Set_Bool("OutReserve", false);
        }
    }

    __super::Update(pOwner, dt);
}

void CJaneDoeState_ExAttack::Exit(CJaneDoe* pOwner)
{
    pOwner->Reset_InputInfo();
    pOwner->Reset_ReserveCombo();
    pOwner->Pop_Invincible();
    __super::Exit(pOwner);
}

void CJaneDoeState_ExAttack_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ExSpecial")
        .Speed(1.2f)
        .EndAt(0.9f)
        .Apply();
}

void CJaneDoeState_ExAttack_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CJaneDoeState_ExAttack_Start::Update_Effects(CJaneDoe* pOwner)
{
    if (IsCrossAnimProgress(0.17f))
        pOwner->Play_Effect("JaneDoe_Ex_Slash0", _vector3(0.f, 1.3f, 0.f), _quaternion(0.64f, 0.29f, 0.64f, 0.29f));

    if (IsCrossAnimProgress(0.24f))
        pOwner->Play_Effect("JaneDoe_Ex_Slash1", _vector3(0.f, 2.f, 0.f), _quaternion(0.64f, 0.29f, 0.64f, 0.29f));

    if (IsCrossAnimProgress(0.31f))
        pOwner->Play_Effect("JaneDoe_Ex_Slash2", _vector3(0.f, 2.4f, 0.f), _quaternion(0.64f, 0.29f, 0.64f, 0.29f));

    if (IsCrossAnimProgress(0.38f))
        pOwner->Play_Effect("JaneDoe_Ex_Slash3", _vector3(0.f, 2.4f, 0.f), _quaternion(0.64f, 0.29f, 0.64f, 0.29f));

    if (IsCrossAnimProgress(0.5f))
        pOwner->Play_Effect("JaneDoe_Sting", _vector3(0.4f, 2.3f, -3.f), _quaternion(0.2f, 0.f, 0.f, 0.98f), false);

    if (IsCrossAnimProgress(0.52f))
        pOwner->Play_Effect("JaneDoe_Normal_Slash0",_vector3(0.2f,0.5f,0.3f),_quaternion(0.59f,0.56f,-0.43f,0.39f));
}

void CJaneDoeState_ExAttack_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ExSpecial_End")
        .Speed(1.5f)
        .Apply();
}
