#include "pch.h"
#include "Corin.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"

#include "Material.h"

#include "Animator3D.h"
#include "CharacterController.h"
#include "ObjectContainer.h"

#include "StateMachine.h"
#include "CorinState_Start.h"
#include "CorinState_Idle.h"
#include "CorinState_Move.h"
#include "CorinState_Attack.h"
#include "CorinState_NormalAttack.h"
#include "CorinState_CounterAttack.h"
#include "CorinState_AssaultAttack.h"
#include "CorinState_Evade.h"
#include "CorinState_SwitchIn.h"
#include "CorinState_SwitchInParryAid.h"
#include "CorinState_SwitchOut.h"
#include "CorinState_Hit.h"

#include "FootIK.h"

#include "DataBase.h"
#include "EffectContainer.h"

CCorin::CCorin()
{
}

CCorin::CCorin(const CCorin& rhs)
    : CCharacter(rhs)
{
}

HRESULT CCorin::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Get_Component<CModel>()->Link_Model(G_GlobalLevelKey, "Corin.model");
    Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Corin.mat");
    return S_OK;
}

HRESULT CCorin::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Initialize_StateMachine()))
        return E_FAIL;
    if (FAILED(Initialize_Weapon()))
        return E_FAIL;

    return S_OK;
}

void CCorin::Awake()
{
    __super::Awake();

    m_pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Corin.model");
    m_pAnimator->Link_MetaData(G_GlobalLevelKey, "Avatar_Female_Size01_Corin_Meta.json");
    m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
    m_strAnimName = "Avatar_Female_Size01_Corin_Ani_";
    m_pAnimator->Set_Animation(Get_Name() + "Idle")
        .Loop(true)
        .Apply();

    m_strName = "Corin";
    m_eCharacterName = CHARACTER::Corin;

    //m_pAnimator->Initialize_HumanoidRig();
    //CFootIK::FOOTIK_DESC ikDesc;
    //ikDesc.fRayStartOffset = 0.3f;
    //ikDesc.fRayDistance = 1.0f;
    //ikDesc.fMaxHeightDiff = 0.5f;
    //ikDesc.fMaxPelvisOffset = 0.1f;
    //ikDesc.iCollisionMask = 1 << ENUM(COLLISION_GROUP::COMMON);
    //ikDesc.bDynamicPoleVector = false;
    //ikDesc.vPoleVector = _vector3(0.f, 1.f, 0.f);
    //m_pAnimator->Initialize_FootIK(&ikDesc);

    Initialize_Stat();
    m_fCurrentHP = 300.f;
    m_tEnergy.fCurrentEnergy = 75;

    if (FAILED(Attach_ParryCollider()))
        return;
}

void CCorin::Priority_Update(_float dt)
{
    __super::Priority_Update(dt);
}

void CCorin::Update(_float dt)
{
    if (!m_bTest)
    {
        Update_States();
        m_pStateMachine->Update(dt);
    }
    __super::Update(dt);
}

void CCorin::Late_Update(_float dt)
{
    __super::Late_Update(dt);
}

void CCorin::Render_GUI()
{
    if (m_pStateMachine)
    {
        ImGui::Separator();
        ImGui::Checkbox("Animation Test", &m_bTest);
        ImGui::Text("StateMachine: %s", m_pStateMachine->Get_CurrentStateName().c_str());

        if (ImGui::Button("Open StateMachine"))
            m_pStateMachine->Set_ShowWindow(true);

        m_pStateMachine->Render_GUI();
    }

    __super::Render_GUI();
}

void CCorin::Reset_State()
{
    m_pStateMachine->Set_Trigger("ResetState");
}

void CCorin::On_Start()
{
    m_pStateMachine->Set_Trigger("QuestStart");
}

void CCorin::On_SwitchIn(SWITCH eType)
{
    m_fDissolveProgress = 0.f;
    SetRenderLayer(RENDER_LAYER::Default);

    Set_Switch(eType);
    m_pStateMachine->Set_Trigger("SwitchIn");
}

void CCorin::On_SwitchOut()
{
    __super::On_SwitchOut();

    if (m_pStateMachine->Get_CurrentStateName() == "Attack")
    {
        m_pStateMachine->Set_Bool("OutReserve", true);
        return;
    }
    else if (m_pStateMachine->Get_CurrentStateName() == "SwitchIn")
    {
        IHState<CCorin>* pState = dynamic_cast<IHState<CCorin>*>(m_pStateMachine->Get_CurrentState());
        CStateMachine<CCorin>* pSub = pState->Get_SubStateMachine();
        if (pSub && pSub->Get_CurrentStateName() != "SwitchInNormal")
        {
            m_pStateMachine->Set_Bool("OutReserve", true);
            return;
        }
    }
    m_pStateMachine->Set_Trigger("SwitchOut");
}

void CCorin::On_Ultimate()
{
    __super::On_Ultimate();
    m_pStateMachine->Set_Int("AttackEntryMode", 3);
    m_pStateMachine->Set_Trigger("Attack");
}

void CCorin::On_Special()
{
    if (InputDevice()->Key_Tap('E') == false) return;

    string strCurrentState = m_pStateMachine->Get_CurrentStateName();

    // NormalAttack 중 캔슬해서 ExAttack
    if (strCurrentState == "Attack")
    {
        CCorinState_Attack* pAttack = static_cast<CCorinState_Attack*>(
            m_pStateMachine->Get_CurrentState());
        if (pAttack && pAttack->Get_SubStateMachine())
        {
            if (pAttack->Get_SubStateMachine()->Get_CurrentStateName() == "NormalAttack")
            {
                pAttack->Get_SubStateMachine()->Set_Trigger("ToExAttack");
                return;
            }
        }
    }

    m_pStateMachine->Set_Int("AttackEntryMode", 2);
    m_pStateMachine->Set_Trigger("Attack");
}

void CCorin::On_Hit(DAMAGE_TYPE eType)
{
    m_pStateMachine->Set_Int("HitEntryMode", ENUM(eType));
    m_pStateMachine->Set_Trigger("ToHit");
}

HRESULT CCorin::Initialize_StateMachine()
{
    m_pStateMachine = CStateMachine<CCorin>::Create();
    if (!m_pStateMachine)
        return E_FAIL;

    if (FAILED(Initialize_States()))
        return E_FAIL;

    if (FAILED(Initialize_Transitions()))
        return E_FAIL;

    if (FAILED(Initialize_Effects()))
        return E_FAIL;

    m_pStateMachine->Set_DefaultState("Idle");
    m_pStateMachine->Initialize(this);

    return S_OK;
}

HRESULT CCorin::Initialize_States()
{
    m_pStateMachine->Register_State("Start", CCorinState_Start::Create());
    m_pStateMachine->Register_State("Idle", CCorinState_Idle::Create());
    m_pStateMachine->Register_State("Move", CCorinState_Move::Create());
    m_pStateMachine->Register_State("Attack", CCorinState_Attack::Create());
    m_pStateMachine->Register_State("Evade", CCorinState_Evade::Create());
    m_pStateMachine->Register_State("SwitchIn", CCorinState_SwitchIn::Create());
    m_pStateMachine->Register_State("SwitchOut", CCorinState_SwitchOut::Create());
    m_pStateMachine->Register_State("Hit", CCorinState_Hit::Create());

    return S_OK;
}

HRESULT CCorin::Initialize_Transitions()
{
    // Start
    m_pStateMachine->Register_AnyStateTransition("Start",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "QuestStart");

    m_pStateMachine->Register_Transition("Start", "Idle",
        CStateMachine<CCorin>::CONDITION_ANIMATION_END);

    // Idle -> Move
    m_pStateMachine->Register_Transition("Idle", "Move",
        CStateMachine<CCorin>::CONDITION_BOOL_TRUE, "IsMove");

    // Move -> Idle
    m_pStateMachine->Register_Transition("Move", "Idle",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

    m_pStateMachine->Register_AnyStateTransition("Idle",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ResetState");

    // Attack
    m_pStateMachine->Register_AnyStateTransition("Attack",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "Attack");

    // Attack -> Idle
    m_pStateMachine->Register_Transition("Attack", "Idle",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

    // Evade
    m_pStateMachine->Register_AnyStateTransition("Evade",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToEvade");

    // Evade -> Move (Dash)
    m_pStateMachine->Register_Transition("Evade", "Move",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToMove");

    // Evade -> Idle (Backstep)
    m_pStateMachine->Register_Transition("Evade", "Idle",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

    // SwitchIn
    m_pStateMachine->Register_AnyStateTransition("SwitchIn",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "SwitchIn");

    m_pStateMachine->Register_Transition("SwitchIn", "Idle",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

    m_pStateMachine->Register_Transition("SwitchIn", "Move",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToMove");

    // SwitchOut
    m_pStateMachine->Register_AnyStateTransition("SwitchOut",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "SwitchOut", 1);

    m_pStateMachine->Register_Transition("SwitchOut", "Idle",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

    // Hit
    vector<CStateMachine<CCorin>::CONDITION_INFO> HitConditions;
    HitConditions.push_back({ CStateMachine<CCorin>::CONDITION_TRIGGER, "ToHit" });
    HitConditions.push_back({ CStateMachine<CCorin>::CONDITION_BOOL_FALSE, "Resistance" });
    m_pStateMachine->Register_AnyStateTransition("Hit", HitConditions);

    m_pStateMachine->Register_Transition("Hit", "Idle",
        CStateMachine<CCorin>::CONDITION_TRIGGER, "ToIdle");

    return S_OK;
}

HRESULT CCorin::Initialize_Stat()
{
    auto Desc = CDataBase::GetInstance()->GetPlayerDesc(m_strName);

    m_fMaxHP = Desc.MaxHP;
    m_fAttackPower = Desc.Attack;
    m_fDefense = Desc.Defend;
    m_tEnergy.fSpecialEnergy = Desc.SpecialAttack;
    Set_EvadeMax(2);

    // 추가 버프 적용
    string outID;
    RuntimeBucket().String.TryGet(PersistScope::SaveSlot, "RamenID", outID);
    if (!outID.empty())
    {
        auto Ramen = CDataBase::GetInstance()->GetRamenDesc(outID);
        for (auto attribute : Ramen.attributes)
        {
            string attID = attribute.strAttributeID;
            if (attID == "atk")
            {
                m_fAttackPower += attribute.iAttributeValue * 0.01f;
            }
            else if (attID == "max_hp")
            {
                m_fMaxHP += attribute.iAttributeValue;
            }
            else if (attID == "dmg_physical")
            {
                m_fAttackPower += attribute.iAttributeValue * 0.01f;
            }
            else
                continue;
        }
    }

    return S_OK;
}

HRESULT CCorin::Initialize_Weapon()
{
    ATTACK_COLLIDER_DESC desc;
    desc.eColliderType = COLLIDER_TYPE::BOX;
    desc.pOwnerAnimator = Get_Component<CAnimator3D>();
    desc.tagBone = "Weapon_saw";
    desc.tagName = "Saw";
    desc.vSize = { 0.7f, 0.7f, 0.2f };

    if (FAILED(Attach_AttackCollider(&desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCorin::Initialize_Effects()
{
    if (FAILED(__super::Initialize_Effects()))
        return E_FAIL;

    auto pObjectContainer = Get_Component<CObjectContainer>();
    auto pAnimator = Get_Component<CAnimator3D>();

    // Normal Slash
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_normal1_slash.json")
            .Build("Corin_Normal_Slash0");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_normal1_slash.json")
            .Build("Corin_Normal_Slash1");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_normal1_slash.json")
            .Build("Corin_Normal_Slash2");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_normal1_slash.json")
            .Build("Corin_Normal_Slash3");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_normal2_slash.json")
            .Build("Corin_Normal2_Slash0");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }

    // Sting
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_normal1_sting.json")
            .Build("Corin_Sting0");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect, false);
    }

    // Saw Slash
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_normal1_saw_slash.json")
            .Build("Corin_Saw_Slash0");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect, false);
        pEffect->AttachBone(pAnimator, "Weapon_saw");
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_normal2_saw_slash.json")
            .Build("Corin_Saw_Slash1");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect, false);
        pEffect->AttachBone(pAnimator, "Weapon_saw");
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_ex_saw_slash.json")
            .Build("Corin_Ex_Saw_Slash0");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect, false);
        pEffect->AttachBone(pAnimator, "Weapon_saw");
    }

    // Ex Explode
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("corin_ex_explode.json")
            .Build("Corin_Ex_Explode");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect, false);
    }

    return S_OK;
}

void CCorin::Update_States()
{
    if (!Is_MainCharacter()) return;
    if (!m_pCCT->Get_CompActive()) return;

    for (const auto& Event : Get_Animator()->Get_EventBus())
    {
        if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
        if (Event.Tag == "CheckCombo")
        {
            if (m_bReserveCombo)
            {
                m_bReserveCombo = false;
                BattleSystem()->GetBattlePlayer()->Request_ComboAttack();
            }
        }
    }

    m_pStateMachine->Set_Bool("IsMove", Is_Move_Buffer());

    Process_EndState(m_pStateMachine->Get_CurrentStateName());

    if (m_bIsEvade)
        m_pStateMachine->Set_Trigger("ToEvade");

    if (m_bIsAttack)
        Process_AttackInput(m_pStateMachine->Get_CurrentStateName());
}

void CCorin::Process_AttackInput(const string& strCurrentState)
{
    if (strCurrentState == "Idle")
    {
        m_pStateMachine->Set_Int("AttackEntryMode", 0);
        m_pStateMachine->Set_Trigger("Attack");
    }
    else if (strCurrentState == "Move")
    {
        CCorinState_Move* pMove = static_cast<CCorinState_Move*>(
            m_pStateMachine->Get_CurrentState());
        if (!pMove || !pMove->Get_SubStateMachine())
            return;

        string strMoveType = pMove->Get_SubStateMachine()->Get_CurrentStateName();

        if (strMoveType == "Run")
        {
            IHState<CCorin>* pRun = static_cast<IHState<CCorin>*>(
                pMove->Get_SubStateMachine()->Get_CurrentState());
            if (!pRun || !pRun->Get_SubStateMachine())
                return;

            if (!pRun->Is_EndState())
                m_pStateMachine->Set_Int("AttackEntryMode", 1);
        }

        m_pStateMachine->Set_Trigger("Attack");
    }
    else if (strCurrentState == "Attack")
    {
        CCorinState_Attack* pAttack = static_cast<CCorinState_Attack*>(
            m_pStateMachine->Get_CurrentState());
        if (!pAttack || !pAttack->Get_SubStateMachine())
            return;

        string strAttackType = pAttack->Get_SubStateMachine()->Get_CurrentStateName();

        if (strAttackType == "NormalAttack")
        {
            CCorinState_NormalAttack* pNormal = static_cast<CCorinState_NormalAttack*>(
                pAttack->Get_SubStateMachine()->Get_State("NormalAttack"));
            if (pNormal && pNormal->Get_SubStateMachine())
                pNormal->Get_SubStateMachine()->Set_Trigger("NextCombo");
        }
        else if (strAttackType == "CounterAttack")
        {
            CCorinState_CounterAttack* pCounter = static_cast<CCorinState_CounterAttack*>(
                pAttack->Get_SubStateMachine()->Get_CurrentState());
            if (!pCounter || !pCounter->Get_SubStateMachine())
                return;
            // Start나 Explode 중에만 예약 가능
            if (!pCounter->Is_EndState())
            {
                pAttack->Get_SubStateMachine()->Set_Int("ComboEntryIndex", 3);
                pCounter->Get_SubStateMachine()->Set_Bool("ReserveNormal", true);
            }
        }
        else if (strAttackType == "AssaultAttack")
        {
            CCorinState_AssaultAttack* pAssault = static_cast<CCorinState_AssaultAttack*>(
                pAttack->Get_SubStateMachine()->Get_CurrentState());
            if (!pAssault || !pAssault->Get_SubStateMachine())
                return;
            pAttack->Get_SubStateMachine()->Set_Int("ComboEntryIndex", 4);
            pAssault->Get_SubStateMachine()->Set_Bool("ReserveNormal", true);
        }
    }
    else if (strCurrentState == "SwitchIn")
    {
        CCorinState_SwitchIn* pSwitchIn = static_cast<CCorinState_SwitchIn*>(
            m_pStateMachine->Get_CurrentState());
        if (!pSwitchIn || !pSwitchIn->Get_SubStateMachine())
            return;

        string strSwitchType = pSwitchIn->Get_SubStateMachine()->Get_CurrentStateName();
        if (strSwitchType == "SwitchInParryAid")
        {
            CCorinState_SwitchInParryAid* pParryAid = static_cast<CCorinState_SwitchInParryAid*>(
                pSwitchIn->Get_SubStateMachine()->Get_CurrentState());
            if (!pSwitchIn || !pSwitchIn->Get_SubStateMachine())
                return;
            if (!pParryAid->Is_EndState())
            {
                pParryAid->Get_SubStateMachine()->Set_Bool("ReserveAssaultAid", true);
            }
        }
    }
}

void CCorin::Process_EndState(const string& strCurrentState)
{
    if (strCurrentState == "Move")
    {
        CCorinState_Move* pMove = static_cast<CCorinState_Move*>(
            m_pStateMachine->Get_CurrentState());
        if (!pMove) return;

        IHState<CCorin>* pMoveType = dynamic_cast<IHState<CCorin>*>(
            pMove->Get_SubStateMachine()->Get_CurrentState());
        if (pMoveType && pMoveType->Is_EndState())
        {
            IBaseState<CCorin>* pEnd = pMoveType->Get_SubStateMachine()->Get_CurrentState();
            if (m_bIsAttack || m_bIsEvade) return;
            if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
                m_pStateMachine->Set_Trigger("ToIdle");
        }
    }
    else if (strCurrentState == "Attack")
    {
        CCorinState_Attack* pAttack = static_cast<CCorinState_Attack*>(
            m_pStateMachine->Get_CurrentState());
        if (!pAttack) return;

        IHState<CCorin>* pAttackType = dynamic_cast<IHState<CCorin>*>(
            pAttack->Get_SubStateMachine()->Get_CurrentState());
        if (pAttackType && pAttackType->Is_EndState())
        {
            IBaseState<CCorin>* pEnd = pAttackType->Get_SubStateMachine()->Get_CurrentState();
            if (m_bIsEvade) return;
            if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
                m_pStateMachine->Set_Trigger("ToIdle");
        }
    }
    else if (strCurrentState == "SwitchIn")
    {
        CCorinState_SwitchIn* pSwitchIn = static_cast<CCorinState_SwitchIn*>(
            m_pStateMachine->Get_CurrentState());
        if (!pSwitchIn) return;

        IHState<CCorin>* pSwitchInType = dynamic_cast<IHState<CCorin>*>(
            pSwitchIn->Get_SubStateMachine()->Get_CurrentState());
        if (pSwitchInType && pSwitchInType->Is_EndState())
        {
            IBaseState<CCorin>* pEnd = pSwitchInType->Get_SubStateMachine()->Get_CurrentState();
            if (m_bIsEvade) return;
            if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
                m_pStateMachine->Set_Trigger("ToIdle");
        }
    }
    else if (strCurrentState == "Hit")
    {
        CCorinState_Hit* pHit = static_cast<CCorinState_Hit*>(
            m_pStateMachine->Get_CurrentState());
        if (!pHit || !pHit->Get_SubStateMachine()) return;

        IBaseState<CCorin>* pHitType = pHit->Get_SubStateMachine()->Get_CurrentState();
        if (pHitType && pHitType->Get_AnimProgress() > 0.3f)
        {
            if (m_bIsEvade) return;
            if (Is_Input() || pHitType->Is_AnimEnd())
                m_pStateMachine->Set_Trigger("ToIdle");
        }
    }
}

CCorin* CCorin::Create()
{
    CCorin* pInstance = new CCorin();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

CGameObject* CCorin::Clone(INIT_DESC* pArg)
{
    CCorin* pInstance = new CCorin(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CCorin::Free()
{
    Safe_Release(m_pStateMachine);
    __super::Free();
}