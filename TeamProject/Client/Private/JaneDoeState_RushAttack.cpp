#include "pch.h"
#include "JaneDoeState_RushAttack.h"
#include "JaneDoe.h"
#include "EffectContainer.h"

/* Component */
#include "ObjectContainer.h"

void CJaneDoeState_RushAttack::Enter(CJaneDoe* pOwner)
{
    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();

        m_pSubStateMachine->Register_State("Rush03_Start", CJaneDoeState_Rush03_Start::Create());
        m_pSubStateMachine->Register_State("Rush03_End", CJaneDoeState_Rush03_End::Create());
        m_pSubStateMachine->Register_State("Rush02_Start", CJaneDoeState_Rush02_Start::Create());
        m_pSubStateMachine->Register_State("Rush02_End", CJaneDoeState_Rush02_End::Create());
        m_pSubStateMachine->Register_State("Rush01_Start", CJaneDoeState_Rush01_Start::Create());
        m_pSubStateMachine->Register_State("Rush01_End", CJaneDoeState_Rush01_End::Create());

        m_pSubStateMachine->Register_Transition("Rush01_Start", "Rush01_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Rush02_Start", "Rush02_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Rush03_Start", "Rush03_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Rush03_Start");
    }

    if (pOwner->Is_Passion())
    {
        m_pSubStateMachine->Set_DefaultState("Rush03_Start");
        m_pSubStateMachine->Get_State("Rush03_End")->Set_Tag("End");
    }
    else if (pOwner->Get_EvadeCount() <= 1)
    {
        m_pSubStateMachine->Set_DefaultState("Rush02_Start");
        m_pSubStateMachine->Get_State("Rush02_End")->Set_Tag("End");
    }
    else
    {
        m_pSubStateMachine->Set_DefaultState("Rush01_Start");
        m_pSubStateMachine->Get_State("Rush01_End")->Set_Tag("End");
    }

    __super::Enter(pOwner);
}

void CJaneDoeState_RushAttack::Update(CJaneDoe* pOwner, _float dt)
{
    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

        if (Event.Tag == "LHandStart")
        {
            pOwner->Begin_AttackCollider("HandWeapon_L", { HIT_TYPE::ONCE, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(10,30), 0, 0 });
        }
        else if (Event.Tag == "LHandEnd")
        {
            pOwner->End_AttackCollider("HandWeapon_L");
        }
        else if (Event.Tag == "RHandStart")
        {
            pOwner->Begin_AttackCollider("HandWeapon_R", { HIT_TYPE::ONCE, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(10,30), 0, 0 });
        }
        else if (Event.Tag == "RHandEnd")
        {
            pOwner->End_AttackCollider("HandWeapon_R");
        }
        else if (Event.Tag == "LFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_L", { HIT_TYPE::ONCE, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(20,40), 0, 0 });
        }
        else if (Event.Tag == "LFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_L");
        }
        else if (Event.Tag == "RFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_R", { HIT_TYPE::ONCE, DAMAGE_TYPE::HARD, Helper::Get_Random_Float(20,40), 0, 0 });
        }
        else if (Event.Tag == "RFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_R");
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

    __super::Update(pOwner, dt);
}

void CJaneDoeState_RushAttack::Exit(CJaneDoe* pOwner)
{
    __super::Exit(pOwner);
}

void CJaneDoeState_Rush01_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_01")
        .Apply();
}

void CJaneDoeState_Rush01_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CJaneDoeState_Rush01_Start::Update_Effects(CJaneDoe* pOwner)
{
    /* Slash1 */
    if (IsCrossAnimProgress(0.1f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash0");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 0.2f, 0.f));
        pEffectTransform->Set_Quaternion(_quaternion(0.64f, 0.f, 0.f, 0.77f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Slash2 */
    if (IsCrossAnimProgress(0.23f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash1");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(-0.1f, 0.5f, 0.f));
        pEffectTransform->Set_Quaternion(_quaternion(0.5f, -0.23f, -0.37f, 0.75f));
        static_cast<CEffectContainer*>(effect)->Play();
    }
}

void CJaneDoeState_Rush01_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_01_End")
        .Apply();
}

void CJaneDoeState_Rush02_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_02")
        .Apply();
}

void CJaneDoeState_Rush02_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CJaneDoeState_Rush02_Start::Update_Effects(CJaneDoe* pOwner)
{
    /* Slash1 */
    if (IsCrossAnimProgress(0.12f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash0");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 1.5f, 0.f));
        pEffectTransform->Set_Quaternion(_quaternion(0.86f, 0.f, 0.f, 0.51f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Slash2 */
    if (IsCrossAnimProgress(0.21f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash1");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 1.3f, 0.f));
        pEffectTransform->Set_Quaternion(_quaternion(0.6f, 0.58f, -0.3f, 0.4f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Sting1 */
    if (IsCrossAnimProgress(0.38f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Sting2"); 
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(-0.1f, 1.8f, -0.7f));
        pEffectTransform->Set_Quaternion(_quaternion(0.36f, 0.1f, -0.05f, 0.93f));
        static_cast<CEffectContainer*>(effect)->Play();
    }
}

void CJaneDoeState_Rush02_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_02_End")
        .Apply();
}

void CJaneDoeState_Rush03_Start::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_03")
        .Apply();
}

void CJaneDoeState_Rush03_Start::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Rush03_End::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Rush_03_End")
        .Apply();
}
