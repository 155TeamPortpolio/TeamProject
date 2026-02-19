#include "pch.h"
#include "MiyabiState_SwitchInParryAid.h"

#include "GameInstance.h"
#include "BattleSystem.h"

#include "Miyabi.h"
#include "Enemy.h"

#include "CamDirector.h"
#include "ObjectContainer.h"
#include "EffectContainer.h"

CMiyabiState_SwitchInParryAid* CMiyabiState_SwitchInParryAid::Create()
{
    auto pInstance = new CMiyabiState_SwitchInParryAid();
    pInstance->m_pSubStateMachine = CStateMachine<CMiyabi>::Create();
    auto pSubStateMachine = pInstance->Get_SubStateMachine();

    pSubStateMachine->Register_State("ParryAid_Start", CMiyabiState_SwitchInParryAid_Start::Create());
    pSubStateMachine->Register_State("L_Loop", CMiyabiState_SwitchInParryAid_L_Loop::Create());
    pSubStateMachine->Register_State("L_End", CMiyabiState_SwitchInParryAid_L_End::Create());
    pSubStateMachine->Register_State("H_Loop", CMiyabiState_SwitchInParryAid_H_Loop::Create());
    pSubStateMachine->Register_State("H_End", CMiyabiState_SwitchInParryAid_H_End::Create());

    pSubStateMachine->Get_State("L_End")->Set_Tag("End");
    pSubStateMachine->Get_State("H_End")->Set_Tag("End");

    pSubStateMachine->Register_Transition("ParryAid_Start", "L_Loop",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ParryImpact");
    // 타임아웃 안전장치 (몬스터 공격이 빗나간 경우)
    pSubStateMachine->Register_Transition("ParryAid_Start", "L_End",
        CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ParryFail");
    pSubStateMachine->Register_Transition("L_Loop", "L_End",
        CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

    pSubStateMachine->Set_DefaultState("ParryAid_Start");

    pSubStateMachine->Set_Bool("ReserveAssaultAid", false);
    return pInstance;
}

void CMiyabiState_SwitchInParryAid::Enter(CMiyabi* pOwner)
{
    pOwner->Lock_Move();
    //pOwner->Lock_Rotate();
    m_pSubStateMachine->Reset_Trigger("ParryImpact");
    m_pSubStateMachine->Reset_Trigger("ParryFail");
    CamDirector()->StartParry();

    __super::Enter(pOwner);
}

void CMiyabiState_SwitchInParryAid::Update(CMiyabi* pOwner, _float dt)
{
    if (m_pSubStateMachine->Get_Trigger("Complete"))
    {
        m_pSubStateMachine->Reset_Trigger("Complete");
        IHState<CMiyabi>* pSwitchIn = Get_ParentState();
        if (pSwitchIn && pSwitchIn->Get_SubStateMachine())
        {
            pSwitchIn->Get_SubStateMachine()->Set_Int("ExitMode", 0);  // Idle
            pSwitchIn->Get_SubStateMachine()->Set_Trigger("Complete");
        }
    }
    pOwner->Look_Target();
    __super::Update(pOwner, dt);
}

void CMiyabiState_SwitchInParryAid::Exit(CMiyabi* pOwner)
{
    pOwner->Unlock_Move();
    pOwner->Unlock_Rotate();
    pOwner->Set_ResetMove(true);
    __super::Exit(pOwner);
}

void CMiyabiState_SwitchInParryAid_Start::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_Start")
        .BlendDuration(0.125f)
        .ReserveSpeed(0.f, 0.24f, 2.f, EaseType::OutQuint)
        .ReserveSpeed(0.24f, 0.25f, 0.f, EaseType::OutQuint)
        .EndAt(0.25f)
        .Apply();

    if (pOwner->Get_StateMachine()->Get_Trigger("ReserveParryImpact"))
    {
        pOwner->Get_StateMachine()->Reset_Trigger("ReserveParryImpact");
        m_pOwnerStateMachine->Set_Trigger("ParryImpact");
    }
}

void CMiyabiState_SwitchInParryAid_Start::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    if (m_fStateTime > 1.5f)  // 1.5초 타임아웃
    {
        m_pOwnerStateMachine->Set_Trigger("ParryFail");
    }
}

void CMiyabiState_SwitchInParryAid_L_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "ParryAid_L")
        .ReserveSpeed(0.f, 1.f, 2.f, EaseType::OutExpo)
        .Apply();

    auto pos = CamDirector()->GetParryPoint();
    auto pParryEffect = pOwner->Get_Component<CObjectContainer>()->Find_ObjectByName("Parry");
    if (pParryEffect)
    {
        pParryEffect->Get_Component<CTransform>()->Set_WorldPos(pos);
        static_cast<CEffectContainer*>(pParryEffect)->Play();
    }
    BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::PARRY);

    //OBJECT_HANDLE handle = pOwner->Get_ParryHandle();
    //if (handle.isValid())
    //{
    //    TARGET_LOCK_DESC desc;
    //    desc.bLock = false;
    //    desc.tHandle = handle;
    //    EventSystem()->Broadcast<TARGET_LOCK_DESC>({desc});
    //}
}

void CMiyabiState_SwitchInParryAid_L_Loop::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_SwitchInParryAid_L_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "ParryAid_L_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CMiyabiState_SwitchInParryAid_L_End::Update(CMiyabi* pOwner, _float dt)
{
    if (m_pOwnerStateMachine->Get_Bool("ReserveAssaultAid"))
    {
        pOwner->Get_StateMachine()->Set_Trigger("Attack");
        pOwner->Get_StateMachine()->Set_Int("AttackEntryMode", 6);
        m_pOwnerStateMachine->Set_Bool("ReserveAssaultAid", false);
    }

    pOwner->Process_RootMotion(dt);

    IHState<CMiyabi>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}

void CMiyabiState_SwitchInParryAid_H_Loop::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_H")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CMiyabiState_SwitchInParryAid_H_Loop::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_SwitchInParryAid_H_End::Enter(CMiyabi* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_ParryAid_H_End")
        .Loop(false)
        .Speed(1.2f)
        .Apply();
}

void CMiyabiState_SwitchInParryAid_H_End::Update(CMiyabi* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
        ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));

    IHState<CMiyabi>* pSwitch = Get_ParentState();
    if (!pSwitch || !pSwitch->Get_SubStateMachine()) return;

    if (m_fAnimProgress >= 0.75f)
        pSwitch->Get_SubStateMachine()->Set_Trigger("Complete");
}
