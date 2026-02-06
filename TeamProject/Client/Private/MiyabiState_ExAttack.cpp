#include "pch.h"
#include "MiyabiState_ExAttack.h"

#include "GameInstance.h"

#include "Miyabi.h"

CMiyabiState_ExAttack* CMiyabiState_ExAttack::Create()
{
    auto pInstance = new CMiyabiState_ExAttack();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("ExAttack_Start", CMiyabiState_ExAttack_Start::Create());
    pSubStateMachine->Register_State("ExAttack_01", CMiyabiState_ExAttack_01::Create());
    pSubStateMachine->Register_State("ExAttack_02", CMiyabiState_ExAttack_02::Create());
    pSubStateMachine->Register_State("ExAttack_03", CMiyabiState_ExAttack_03::Create());
    pSubStateMachine->Register_State("ExAttack_End", CMiyabiState_ExAttack_End::Create());

    pSubStateMachine->Get_State("ExAttack_End")->Set_Tag("End");

    vector<CStateMachine<CMiyabi>::CONDITION_INFO> conditions;

    conditions.push_back({ CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "Enhanced" });
    conditions.push_back({ CStateMachine<CMiyabi>::CONDITION_ANIMATION_END });
    pSubStateMachine->Register_Transition("ExAttack_Start", "ExAttack_01",
        conditions);
    conditions.clear();

    conditions.push_back({ CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "Enhanced" });
    conditions.push_back({ CStateMachine<CMiyabi>::CONDITION_ANIMATION_END });
    pSubStateMachine->Register_Transition("ExAttack_Start", "ExAttack_02",
        conditions);
    conditions.clear();

    conditions.push_back({ CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "Extra" });
    conditions.push_back({ CStateMachine<CMiyabi>::CONDITION_ANIMATION_END });
    pSubStateMachine->Register_Transition("ExAttack_02", "ExAttack_03",
        conditions);
    conditions.clear();

    pSubStateMachine->Register_Transition("ExAttack_01", "ExAttack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("ExAttack_02", "ExAttack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);
    pSubStateMachine->Register_Transition("ExAttack_03", "ExAttack_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("ExAttack_Start");

    return pInstance;
}

void CMiyabiState_ExAttack::Enter(CMiyabi* pOwner)
{
    m_pSubStateMachine->Set_Bool("Extra", false);

    // 강화 상태 판정
    auto EnergyDesc = pOwner->Get_EnergyDesc();
    _bool bEnhanced = EnergyDesc.fCurrentEnergy >= EnergyDesc.fSpecialEnergy;
    m_pSubStateMachine->Set_Bool("Enhanced", bEnhanced);
    if (bEnhanced)
    {
        EnergyDesc.fCurrentEnergy -= EnergyDesc.fSpecialEnergy;
        pOwner->Set_CurrentEnergy(EnergyDesc.fCurrentEnergy);
        pOwner->Push_Invincible();

        UI_ACTION_DESC desc;
        desc.eType = UI_ACTION_TYPE::SPECIAL;
        if (EnergyDesc.fCurrentEnergy >= EnergyDesc.fSpecialEnergy)
        {
            desc.eState = UI_ACTION_STATE::AVAILABLE;
        }
        else
        {
            desc.eState = UI_ACTION_STATE::EXECUTING;
        }
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }

    pOwner->Get_StateMachine()->Set_Bool("Resistance", true);
    __super::Enter(pOwner);
}

void CMiyabiState_ExAttack::Update(CMiyabi* pOwner, _float dt)
{
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

void CMiyabiState_ExAttack::Exit(CMiyabi* pOwner)
{
    pOwner->Reset_ReserveCombo();
    pOwner->Get_StateMachine()->Set_Bool("Resistance", false);
    pOwner->Pop_Invincible();
    __super::Exit(pOwner);
}

void CMiyabiState_ExAttack_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_01_Start")
        .Speed(1.f)
        .Apply();
}

void CMiyabiState_ExAttack_Start::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_ExAttack_01::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_01_Attack_01")
        .Speed(1.f)
        .Apply();
}

void CMiyabiState_ExAttack_01::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CMiyabiState_ExAttack_01::Update_Effects(CMiyabi* pOwner)
{
    if (IsCrossAnimProgress(0.17f))
    {
        pOwner->Play_Effect("Miyabi_Ex0_Slash0", _vector3(0.f, 0.7f, 0.f), _quaternion(-0.21f, 0.71f, 0.64f, 0.2f));
        pOwner->Play_Effect("Miyabi_Ex0_Sting0", _vector3(-6.6f, 0.8f, 2.5f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
    }
}

void CMiyabiState_ExAttack_02::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_01_Attack_02")
        .Speed(1.f)
        .Apply();
}

void CMiyabiState_ExAttack_02::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    if (m_fAnimProgress >= 0.35f)
    {
        if (InputDevice()->Key_Down('E'))
        {
            auto EnergyDesc = pOwner->Get_EnergyDesc();
            if (EnergyDesc.fCurrentEnergy >= EnergyDesc.fSpecialEnergy)
            {
                m_pOwnerStateMachine->Set_Bool("Extra", true);
            }
        }
    }
}

void CMiyabiState_ExAttack_03::Enter(CMiyabi* pOwner)
{
    auto EnergyDesc = pOwner->Get_EnergyDesc();
    EnergyDesc.fCurrentEnergy -= EnergyDesc.fSpecialEnergy;
    pOwner->Set_CurrentEnergy(EnergyDesc.fCurrentEnergy);
    UI_ACTION_DESC desc;
    desc.eType = UI_ACTION_TYPE::SPECIAL;
    if (EnergyDesc.fCurrentEnergy >= EnergyDesc.fSpecialEnergy)
    {
        desc.eState = UI_ACTION_STATE::AVAILABLE;
    }
    else
    {
        desc.eState = UI_ACTION_STATE::EXECUTING;
    }
    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });

    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Branch_01_Attack_03")
        .Speed(1.f)
        .Apply();
}

void CMiyabiState_ExAttack_03::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_ExAttack_End::Enter(CMiyabi* pOwner)
{
    const string arrEndAnims[3] =
    {
        pOwner->Get_Name() + "Attack_Branch_01_Attack_01_End",
        pOwner->Get_Name() + "Attack_Branch_01_Attack_02_End",
        pOwner->Get_Name() + "Attack_Branch_01_Attack_03_End",
    };
    _int iIndex = {};
    if (!m_pOwnerStateMachine->Get_Bool("Enhanced"))
        iIndex = 0;
    else
    {
        if (!m_pOwnerStateMachine->Get_Bool("Extra"))
            iIndex = 1;
        else
            iIndex = 2;
    }
    pOwner->Get_Animator()->Change_Animation(arrEndAnims[iIndex])
        .Apply();
}
