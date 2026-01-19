#include "pch.h"
#include "JaneDoeState_NormalAttack.h"
#include "JaneDoe.h"
#include "GameInstance.h"
#include "Animator3D.h"
#include "ObjectContainer.h"
#include "EffectContainer.h"

void CJaneDoeState_NormalAttack::Enter(CJaneDoe* pOwner)
{
    pOwner->Lock_Move();
    m_iComboIndex = 0;

    if (!m_pSubStateMachine)
    {
        m_pSubStateMachine = CStateMachine<CJaneDoe>::Create();

        m_pSubStateMachine->Register_State("Attack_01", CJaneDoeState_Attack_01::Create());
        m_pSubStateMachine->Register_State("Attack_02", CJaneDoeState_Attack_02::Create());
        m_pSubStateMachine->Register_State("Attack_03", CJaneDoeState_Attack_03::Create());
        m_pSubStateMachine->Register_State("Attack_04", CJaneDoeState_Attack_04::Create());
        m_pSubStateMachine->Register_State("Attack_05", CJaneDoeState_Attack_05::Create());
        m_pSubStateMachine->Register_State("Attack_06", CJaneDoeState_Attack_06::Create());
        m_pSubStateMachine->Register_State("Attack_End", CJaneDoeState_Attack_End::Create());

        m_pSubStateMachine->Get_State("Attack_End")->Set_Tag("End");


        // 콤보 전이: Trigger + AnimEnd : 애니매이션중 마우스가 눌렸고 애니매이션이 끝나면 다음 재생
        vector<CStateMachine<CJaneDoe>::CONDITION_INFO> comboConditions;
        comboConditions.push_back({ CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "NextCombo", 0.f });
        comboConditions.push_back({ CStateMachine<CJaneDoe>::CONDITION_ANIMATION_GREATER, "", 0.6f });

        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_02", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_03", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_04", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_05", comboConditions);
        m_pSubStateMachine->Register_Transition("Attack_05", "Attack_06", comboConditions);


        // End 전이
        m_pSubStateMachine->Register_Transition("Attack_01", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_02", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_03", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_04", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_05", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);
        m_pSubStateMachine->Register_Transition("Attack_06", "Attack_End",
            CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

        m_pSubStateMachine->Set_DefaultState("Attack_01");
    }

    // 트리거 초기화
    m_pSubStateMachine->Reset_Trigger("NextCombo");
    __super::Enter(pOwner);
}

void CJaneDoeState_NormalAttack::Update(CJaneDoe* pOwner, _float dt)
{
    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Tap(MOUSE_BTN::LB))
        m_pSubStateMachine->Set_Trigger("NextCombo");

    for (const auto& Event : pOwner->Get_Component<CAnimator3D>()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;

        if (Event.Tag == "LHandStart")
        {
            pOwner->Begin_AttackCollider("HandWeapon_L", {HIT_TYPE::ONCE, DAMAGE_TYPE::NORMAL, Helper::Get_Random_Float(10,30), 0, 0});
        }
        else if (Event.Tag == "LHandEnd")
        {
            pOwner->End_AttackCollider("HandWeapon_L");
        }
        else if (Event.Tag == "RHandStart")
        {
            pOwner->Begin_AttackCollider("HandWeapon_R", { HIT_TYPE::ONCE, DAMAGE_TYPE::NORMAL, Helper::Get_Random_Float(10,30), 0, 0 });
        }
        else if (Event.Tag == "RHandEnd")
        {
            pOwner->End_AttackCollider("HandWeapon_R");
        }
        else if (Event.Tag == "LFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_L", { HIT_TYPE::ONCE, DAMAGE_TYPE::NORMAL, Helper::Get_Random_Float(20,40), 0, 0 });
        }
        else if (Event.Tag == "LFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_L");
        }
        else if (Event.Tag == "RFootStart")
        {
            pOwner->Begin_AttackCollider("FootWeapon_R", { HIT_TYPE::ONCE, DAMAGE_TYPE::NORMAL, Helper::Get_Random_Float(20,40), 0, 0 });
        }
        else if (Event.Tag == "RFootEnd")
        {
            pOwner->End_AttackCollider("FootWeapon_R");
        }
    }

    __super::Update(pOwner, dt);
}

void CJaneDoeState_NormalAttack::Exit(CJaneDoe* pOwner)
{
    __super::Exit(pOwner);
}


void CJaneDoeState_Attack_01::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_01")
        .Speed(1.2f)
        .ReserveSpeed(0.8, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_01::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CJaneDoeState_Attack_01::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(0);
}

void CJaneDoeState_Attack_01::Update_Effects(CJaneDoe* pOwner)
{
    /* Slash */
    if (IsCrossAnimProgress(0.16f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash0");
        auto pEffectTransform = effect->Get_Component<CTransform>();
        
        pEffectTransform->Set_Pos(_vector3(0.f, 0.6f, -0.4f));
        pEffectTransform->Set_Quaternion(_quaternion(0.62f, 0.28f, -0.15f, 0.72f));
        static_cast<CEffectContainer*>(effect)->Play();
    }
}

void CJaneDoeState_Attack_02::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_02")
        .Speed(1.2f)
        .ReserveSpeed(0.6, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_02::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CJaneDoeState_Attack_02::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(1);
}

void CJaneDoeState_Attack_02::Update_Effects(CJaneDoe* pOwner)
{
    /* Slash1 */
    if (IsCrossAnimProgress(0.03f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash0");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 0.6f, -0.4f));
        pEffectTransform->Set_Quaternion(_quaternion(0.51f, 0.37f, 0.56f, -0.54f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Slash2 */
    if (IsCrossAnimProgress(0.15f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash1");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 0.6f, -0.4f));
        pEffectTransform->Set_Quaternion(_quaternion(-0.06f, 0.83f, 0.38f, 0.4f));
        static_cast<CEffectContainer*>(effect)->Play();
    }
}

void CJaneDoeState_Attack_03::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_03")
        .Speed(1.2f)
        .ReserveSpeed(0.6, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_03::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CJaneDoeState_Attack_03::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(2);
}

void CJaneDoeState_Attack_03::Update_Effects(CJaneDoe* pOwner)
{
    /* Slash1 */
    if (IsCrossAnimProgress(0.15f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash0");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(-0.1f, 0.6f, 0.4f));
        pEffectTransform->Set_Quaternion(_quaternion(0.36f, 0.61f, -0.38f, 0.59f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Slash Cross */
    if (IsCrossAnimProgress(0.3f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Cross_Slash");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 0.7f, 0.3f));
        pEffectTransform->Set_Quaternion(_quaternion(0.f, 0.f, 0.f, 1.f));
        static_cast<CEffectContainer*>(effect)->Play();
    }
}

void CJaneDoeState_Attack_04::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_04")
        .Speed(1.2f)
        .ReserveSpeed(0.8, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_04::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CJaneDoeState_Attack_04::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(3);
}

void CJaneDoeState_Attack_04::Update_Effects(CJaneDoe* pOwner)
{
    /* Slash1 */
    if (IsCrossAnimProgress(0.1f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash0");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 1.1f, 0.f));
        pEffectTransform->Set_Quaternion(_quaternion(0.07f, 0.76f, -0.6f, 0.f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Slash2 */
    if (IsCrossAnimProgress(0.29f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash1");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 1.6f, -0.5f));
        pEffectTransform->Set_Quaternion(_quaternion(0.7f, 0.25f, -0.3f, 0.57f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Slash3 */
    if (IsCrossAnimProgress(0.36f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash2");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 1.5f, 0.f));
        pEffectTransform->Set_Quaternion(_quaternion(0.67f, -0.05f, -0.05f, 0.74f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Slash4 */
    if (IsCrossAnimProgress(0.41f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash3");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 1.5f, 0.f));
        pEffectTransform->Set_Quaternion(_quaternion(-0.04f, 0.74f, -0.67f, 0.02f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Slash5 */
    if (IsCrossAnimProgress(0.56f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash4");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 1.2f, 0.f));
        pEffectTransform->Set_Quaternion(_quaternion(-0.56f, -0.18f, 0.78f, -0.16f));
        static_cast<CEffectContainer*>(effect)->Play();
    }
}

void CJaneDoeState_Attack_05::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_05")
        .Speed(1.2f)
        .ReserveSpeed(0.6, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_05::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CJaneDoeState_Attack_05::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(4);
}

void CJaneDoeState_Attack_05::Update_Effects(CJaneDoe* pOwner)
{
    /* Wide Slash */
    if (IsCrossAnimProgress(0.1f))
    {
        auto pTransform = pOwner->Get_Component<CTransform>();

        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        _smatrix worldMatrix = pTransform->Get_WorldMatrix();
        _vector3 vWorldPosition = _vector3::Transform(_vector3(0.f, 0.f, 1.6f), worldMatrix);

        _quaternion localQuaternion = _quaternion(-0.23f, 0.67f, 0.66f, 0.24f);
        _quaternion worldQuaternion = pTransform->Get_QuaternionRotate();
        localQuaternion *= worldQuaternion;

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Wide_Slash");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_WorldPos(vWorldPosition);
        pEffectTransform->Set_WorldQuaternion(localQuaternion);

        static_cast<CEffectContainer*>(effect)->Play();
    }
}

void CJaneDoeState_Attack_06::Enter(CJaneDoe* pOwner)
{
    pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "Attack_Normal_06")
        .Speed(1.2f)
        .ReserveSpeed(0.9, 1.0, 0.6, EaseType::Linear)
        .Apply();
}

void CJaneDoeState_Attack_06::Update(CJaneDoe* pOwner, _float dt)
{
    pOwner->Process_RootMotion(dt,
        ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
        ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));

    Update_Effects(pOwner);
}

void CJaneDoeState_Attack_06::Exit(CJaneDoe* pOwner)
{
    static_cast<CJaneDoeState_NormalAttack*>(m_pParentState)->Set_ComboIndex(5);
}

void CJaneDoeState_Attack_06::Update_Effects(CJaneDoe* pOwner)
{
    /* Slash1 */
    if (IsCrossAnimProgress(0.16f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash0");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.3f, 1.1f, 0.1f));
        pEffectTransform->Set_Quaternion(_quaternion(0.52f, -0.4f, -0.31f, 0.69f));
        static_cast<CEffectContainer*>(effect)->Play();
    }
    
    /* Slash1 */
    if (IsCrossAnimProgress(0.26f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash1");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(-0.6f, 2.1f, -0.3f));
        pEffectTransform->Set_Quaternion(_quaternion(0.48f, 0.15f, -0.51f, 0.7f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Slash2 */
    if (IsCrossAnimProgress(0.4f))
    {
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Normal_Slash2");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_Pos(_vector3(0.f, 2.3f, -0.2f));
        pEffectTransform->Set_Quaternion(_quaternion(0.76f, -0.22f, -0.25f, 0.55f));
        static_cast<CEffectContainer*>(effect)->Play();
    }

    /* Sting */
    if (IsCrossAnimProgress(0.63f))
    {
        auto pTransform = pOwner->Get_Component<CTransform>();

        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        _smatrix worldMatrix = pTransform->Get_WorldMatrix();
        _vector3 vWorldPosition = _vector3::Transform(_vector3(0.f, 3.5f, -1.4f), worldMatrix);

        _quaternion localQuaternion = _quaternion(0.28f, 0.f, 0.f, 0.96f);
        _quaternion worldQuaternion = pTransform->Get_QuaternionRotate();
        localQuaternion *= worldQuaternion;

        auto effect = pObjectContainer->Find_ObjectByName("JaneDoe_Sting");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_WorldPos(vWorldPosition);
        pEffectTransform->Set_WorldQuaternion(localQuaternion);

        static_cast<CEffectContainer*>(effect)->Play();
    }
}

void CJaneDoeState_Attack_End::Enter(CJaneDoe* pOwner)
{
    CJaneDoeState_NormalAttack* pParent = static_cast<CJaneDoeState_NormalAttack*>(m_pParentState);
    _uint iIndex = pParent ? pParent->Get_ComboIndex() : 0;

    const string arrEndAnims[6] =
    {
        pOwner->Get_Name() + "Attack_Normal_01_End",
        pOwner->Get_Name() + "Attack_Normal_02_End",
        pOwner->Get_Name() + "Attack_Normal_03_End",
        pOwner->Get_Name() + "Attack_Normal_04_End",
        pOwner->Get_Name() + "Attack_Normal_05_End",
        pOwner->Get_Name() + "Attack_Normal_06_End"
    };

    pOwner->Get_Animator()->Change_Animation(arrEndAnims[iIndex])
        .Speed(1.2f)
        .Apply();
    pOwner->Unlock_Move();
}