#include "pch.h"
#include "JaneDoe.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"

#include "DataBase.h"
#include "EffectContainer.h"

#include "Material.h"
#include "MaterialInstance.h"

#include "SkeletalModel.h"
#include "Animator3D.h"
#include "CharacterController.h"
#include "ObjectContainer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

#include "StateMachine.h"
#include "JaneDoeState_Start.h"
#include "JaneDoeState_Idle.h"
#include "JaneDoeState_Move.h"
#include "JaneDoeState_Attack.h"
#include "JaneDoeState_SwitchIn.h"
#include "JaneDoeState_SwitchInParryAid.h"
#include "JaneDoeState_SwitchOut.h"
#include "JaneDoeState_NormalAttack.h"
#include "JaneDoeState_Hit.h"
#include "JaneDoeState_Evade.h"

CJaneDoe::CJaneDoe()
{
}

CJaneDoe::CJaneDoe(const CJaneDoe& rhs)
    : CCharacter(rhs)
{
}

HRESULT CJaneDoe::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Get_Component<CModel>()->Link_Model(G_GlobalLevelKey, "JaneDoeModel.model");
    Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "JaneDoe.mat");

    // 이펙트 리소스 임시 로드
    {
        ResourceManager()->Add_ResourcePath("janedoe_normal1_slash.json", "../Bin/Resources/Effect/Data/JaneDoe/janedoe_normal1_slash.json");
        ResourceManager()->Add_ResourcePath("janedoe_normal2_slash.json", "../Bin/Resources/Effect/Data/JaneDoe/janedoe_normal2_slash.json");
        ResourceManager()->Add_ResourcePath("janedoe_normal3_slash.json", "../Bin/Resources/Effect/Data/JaneDoe/janedoe_normal3_slash.json");
        ResourceManager()->Add_ResourcePath("janedoe_normal1_sting.json", "../Bin/Resources/Effect/Data/JaneDoe/janedoe_normal1_sting.json");
        ResourceManager()->Add_ResourcePath("janedoe_normal2_sting.json", "../Bin/Resources/Effect/Data/JaneDoe/janedoe_normal2_sting.json");

        ResourceManager()->Add_ResourcePath("Eff_MeleeTrail_078_YZ_05.png", "../Bin/Resources/Effect/Texture/Eff_MeleeTrail_078_YZ_05.png");
        ResourceManager()->Add_ResourcePath("smoke2.png", "../Bin/Resources/Effect/Texture/smoke2.png");
        ResourceManager()->Add_ResourcePath("Dissolve.png", "../Bin/Resources/Effect/Texture/Dissolve.png");
        ResourceManager()->Add_ResourcePath("smoke0.png", "../Bin/Resources/Effect/Texture/smoke0.png");
        ResourceManager()->Add_ResourcePath("Eff_Trail_140_LYF_01.png", "../Bin/Resources/Effect/Texture/Eff_Trail_140_LYF_01.png");

        ResourceManager()->Add_ResourcePath("JaneDoe_Slash0.model", "../Bin/Resources/Effect/Model/JaneDoe_Slash0/JaneDoe_Slash0.model");
        ResourceManager()->Add_ResourcePath("JaneDoe_Slash0.mat", "../Bin/Resources/Effect/Model/JaneDoe_Slash0/JaneDoe_Slash0.mat");
        ResourceManager()->Add_ResourcePath("JaneDoe_Slash1.model", "../Bin/Resources/Effect/Model/JaneDoe_Slash1/JaneDoe_Slash1.model");
        ResourceManager()->Add_ResourcePath("JaneDoe_Slash1.mat", "../Bin/Resources/Effect/Model/JaneDoe_Slash1/JaneDoe_Slash1.mat");
        ResourceManager()->Add_ResourcePath("JaneDoe_Sting0.model", "../Bin/Resources/Effect/Model/JaneDoe_Sting0/JaneDoe_Sting0.model");
        ResourceManager()->Add_ResourcePath("JaneDoe_Sting0.mat", "../Bin/Resources/Effect/Model/JaneDoe_Sting0/JaneDoe_Sting0.mat");
    }

    return S_OK;
}

HRESULT CJaneDoe::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Initialize_StateMachine()))
        return E_FAIL;

    if (FAILED(Initialize_Weapon()))
        return E_FAIL;

    if (FAILED(Initialize_Effects()))
        return E_FAIL;

    return S_OK;
}

void CJaneDoe::Awake()
{
    __super::Awake();

    m_pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "JaneDoeModel.model");
    m_pAnimator->Link_MetaData(G_GlobalLevelKey, "JaneDoe_Meta.json");
    m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
    m_strAnimName = "Avatar_Female_Size03_JaneDoe_Ani_";
    m_pAnimator->Set_Animation(Get_Name() + "Idle")
        .Loop(true)
        .Apply();

    m_strName = "JaneDoe";
    m_eCharacterName = CHARACTER::JaneDoe;

    Initialize_Stat();
    m_fCurrentHP = 300.f;
    m_tEnergy.fCurrentEnergy = 75;

    if (FAILED(Attach_ParryCollider()))
        return;

    auto Texture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Eff_Noise_045.png");
    RenderSystem()->Set_NoiseTexture(NOISE_FXTYPE::MOTIONBLUR, Texture);
}

void CJaneDoe::Priority_Update(_float dt)
{
    if (m_bPassion)
    {
        Update_MotionBlurQueue();
        Add_PassionMotionBlur();
    }
    __super::Priority_Update(dt);
}

void CJaneDoe::Update(_float dt)
{
    if (!m_bTest)
    {
        Update_States();
        m_pStateMachine->Update(dt);
    }
    __super::Update(dt);
}

void CJaneDoe::Late_Update(_float dt)
{
    __super::Late_Update(dt);
}

void CJaneDoe::Render_GUI()
{
    ImGui::Separator();
    ImGui::Text("Passion: %s", (m_bPassion ? "ON" : "OFF"));
    ImGui::Text("Passion Stream : %3.1f", m_fPassionStream);
    if (!m_bPassion)
    {
        if (ImGui::Button("Enter Passion"))
            Increase_Passion(100.f);
    }
    else
    {
        if (ImGui::Button("Exit Passion"))
            Decrease_Passion(100.f);
    }

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

void CJaneDoe::Increase_Passion(_float fStream)
{
    m_fPassionStream += fStream;
    m_fPassionStream = min(m_fPassionStream, MAX_PASSIONSTREAM);
    if (!m_bPassion && m_fPassionStream == MAX_PASSIONSTREAM)
    {
        m_bPassion = true;
        m_bCanSalchow = true;
        m_fAttackPower *= 1.25f;
    }
}

void CJaneDoe::Decrease_Passion(_float fStream)
{
    m_fPassionStream -= fStream;
    m_fPassionStream = max(m_fPassionStream, 0.f);
    if (m_bPassion && m_fPassionStream == 0.f)
    {
        m_bPassion = false;
        m_vRimLightColor = _float3(0.f, 0.f, 0.f);
        m_fRimLightPower = 0.f;
        m_fAttackPower /= 1.25f;
    }
}

void CJaneDoe::Reset_State()
{
    m_pStateMachine->Set_Trigger("ResetState");
}

void CJaneDoe::On_Start()
{
    m_pStateMachine->Set_Trigger("QuestStart");
}

void CJaneDoe::On_SwitchIn(SWITCH eType)
{
    m_fDissolveProgress = 0.f;
    SetRenderLayer(RENDER_LAYER::Default);

    Set_Switch(eType);
    m_pStateMachine->Set_Trigger("SwitchIn");
}

void CJaneDoe::On_SwitchOut()
{
    __super::On_SwitchOut();

    if (m_pStateMachine->Get_CurrentStateName() == "Attack")
    {
        m_pStateMachine->Set_Bool("OutReserve", true);
        return;
    }
    else if (m_pStateMachine->Get_CurrentStateName() == "SwitchIn")
    {
        IHState<CJaneDoe>* pState = dynamic_cast<IHState<CJaneDoe>*>(m_pStateMachine->Get_CurrentState());
        CStateMachine<CJaneDoe>* pSub = pState->Get_SubStateMachine();
        if (pSub && pSub->Get_CurrentStateName() != "SwitchInNormal")
        {
            m_pStateMachine->Set_Bool("OutReserve", true);
            return;
        }
    }
    m_pStateMachine->Set_Trigger("SwitchOut");
}

void CJaneDoe::On_Ultimate()
{
    IHState<CJaneDoe>* pState = dynamic_cast<IHState<CJaneDoe>*>(m_pStateMachine->Get_CurrentState());
    if (pState)
    {
        CStateMachine<CJaneDoe>* pSub = pState->Get_SubStateMachine();
        if (pSub && pSub->Get_CurrentStateName() == "UltimateAttack")
            return;
    }

    __super::On_Ultimate();
    m_pStateMachine->Set_Int("AttackEntryMode", 3);
    m_pStateMachine->Set_Trigger("Attack");
}

void CJaneDoe::On_Special()
{
    if (m_tEnergy.fCurrentEnergy < m_tEnergy.fSpecialEnergy) return;
    if (InputDevice()->Key_Tap('E') == false) return;

    string strCurrentState = m_pStateMachine->Get_CurrentStateName();

    // NormalAttack 중 캔슬해서 ExAttack
    if (strCurrentState == "Attack")
    {
        CJaneDoeState_Attack* pAttack = static_cast<CJaneDoeState_Attack*>(
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

void CJaneDoe::On_Hit(DAMAGE_TYPE eType)
{
    m_pStateMachine->Set_Int("HitEntryMode", ENUM(eType));
    m_pStateMachine->Set_Trigger("ToHit");
}

void CJaneDoe::OnDamage()
{
    // 열광 상태가 아닐때 열광누적
    // 열광 상태일때 열광 소모
    if (!m_bPassion)
        Increase_Passion(1.f);
    else
    {
        // 열광 상태의 살코점프 적중시엔 열광 증가
        CJaneDoeState_Attack* pAttack = static_cast<CJaneDoeState_Attack*>(
            m_pStateMachine->Get_CurrentState());
        if (!pAttack || !pAttack->Get_SubStateMachine())
            return;
        string strAttacktype = pAttack->Get_SubStateMachine()->Get_CurrentStateName();

        if (strAttacktype == "BranchAttack")
            Increase_Passion(1.f);
        else
            Decrease_Passion(1.f);
    }
}

void CJaneDoe::OnPerfectDodge()
{
    Increase_Passion(1.f);
}

void CJaneDoe::OnDefensiveAssist()
{
    Increase_Passion(1.f);
}

void CJaneDoe::Play_Effect(const string& effectTag, _fvector offsetPosition, _fvector offsetQuaternion, _bool syncTransform)
{
    auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName(effectTag);
    if (!pEffect)
        return;

    auto pEffectTransform = pEffect->Get_Component<CTransform>();
    if (syncTransform)
    {
        pEffectTransform->Set_Pos(_vector3(offsetPosition));
        pEffectTransform->Set_Quaternion(offsetQuaternion);
    }
    else
    {
        _smatrix worldMatrix = m_pTransform->Get_WorldMatrix();
        _quaternion worldQuaternion = m_pTransform->Get_QuaternionRotate();

        _vector3 vWorldPosition = _vector3::Transform(offsetPosition, worldMatrix);
        _quaternion localQuaternion(offsetQuaternion);
        localQuaternion *= worldQuaternion;

        pEffectTransform->Set_WorldPos(vWorldPosition);
        pEffectTransform->Set_WorldQuaternion(localQuaternion);
    }

    static_cast<CEffectContainer*>(pEffect)->Play();
}

void CJaneDoe::Update_MotionBlurQueue()
{
    ++m_iFrameCount;
    if (m_iFrameCount < FRAMECOUNT)
        return;

    m_iFrameCount = 0;
    if (m_BoneMatrices.size() > 5)
    {
        m_BoneMatrices.pop_front();
        m_WorldMatrices.pop_front();
    }

    auto Model = Get_Component<CSkeletalModel>();
    vector<vector<_float4x4>> BoneMatrices;
    BoneMatrices.resize(Model->Get_MeshCount());

    for (_int i = 0; i < Model->Get_MeshCount(); ++i)
    {
        BoneMatrices[i] = m_pAnimator->Get_BoneMatrices(i);
    }
    _float4x4 worldMatrix = *m_pTransform->Get_WorldMatrix_Ptr();
    m_WorldMatrices.push_back(worldMatrix);
    m_BoneMatrices.push_back(BoneMatrices);
}

HRESULT CJaneDoe::Initialize_StateMachine()
{
    m_pStateMachine = CStateMachine<CJaneDoe>::Create();
    if (!m_pStateMachine)
        return E_FAIL;

    if (FAILED(Initialize_States()))
        return E_FAIL;

    if (FAILED(Initialize_Transitions()))
        return E_FAIL;

    m_pStateMachine->Set_DefaultState("Idle");
    m_pStateMachine->Initialize(this);

    return S_OK;
}

HRESULT CJaneDoe::Initialize_States()
{
    m_pStateMachine->Register_State("Start", CJaneDoeState_Start::Create());
    m_pStateMachine->Register_State("Idle", CJaneDoeState_Idle::Create());
    m_pStateMachine->Register_State("Move", CJaneDoeState_Move::Create());
    m_pStateMachine->Register_State("Attack", CJaneDoeState_Attack::Create());
    m_pStateMachine->Register_State("Evade", CJaneDoeState_Evade::Create());
    m_pStateMachine->Register_State("SwitchIn", CJaneDoeState_SwitchIn::Create());
    m_pStateMachine->Register_State("SwitchOut", CJaneDoeState_SwitchOut::Create());
    m_pStateMachine->Register_State("Hit", CJaneDoeState_Hit::Create());

    return S_OK;
}

HRESULT CJaneDoe::Initialize_Transitions()
{
    // Start
    m_pStateMachine->Register_AnyStateTransition("Start",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "QuestStart");

    m_pStateMachine->Register_Transition("Start", "Idle",
        CStateMachine<CJaneDoe>::CONDITION_ANIMATION_END);

    // Idle -> Move
    m_pStateMachine->Register_Transition("Idle", "Move",
        CStateMachine<CJaneDoe>::CONDITION_BOOL_TRUE, "IsMove");

    // Move -> Idle
    m_pStateMachine->Register_Transition("Move", "Idle",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

    m_pStateMachine->Register_AnyStateTransition("Idle",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ResetState");

    // Attack
    m_pStateMachine->Register_AnyStateTransition("Attack",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "Attack");

    // Attack -> Idle
    m_pStateMachine->Register_Transition("Attack", "Idle",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

    // Evade
    m_pStateMachine->Register_AnyStateTransition("Evade",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToEvade");

    // Evade -> Move (Dash)
    m_pStateMachine->Register_Transition("Evade", "Move",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToMove");

    // Evade -> Idle (Backstep)
    m_pStateMachine->Register_Transition("Evade", "Idle",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

    // SwitchIn
    m_pStateMachine->Register_AnyStateTransition("SwitchIn",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "SwitchIn");

    m_pStateMachine->Register_Transition("SwitchIn", "Idle",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

    m_pStateMachine->Register_Transition("SwitchIn", "Move",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToMove");

    // SwitchOut
    m_pStateMachine->Register_AnyStateTransition("SwitchOut",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "SwitchOut", 1);

    m_pStateMachine->Register_Transition("SwitchOut", "Idle",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

    // Hit
    vector<CStateMachine<CJaneDoe>::CONDITION_INFO> HitConditions;
    HitConditions.push_back({ CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToHit" });
    HitConditions.push_back({ CStateMachine<CJaneDoe>::CONDITION_BOOL_FALSE, "Resistance" });
    m_pStateMachine->Register_AnyStateTransition("Hit", HitConditions);

    m_pStateMachine->Register_Transition("Hit", "Idle",
        CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

    return S_OK;
}

HRESULT CJaneDoe::Initialize_Stat()
{
    auto Desc = CDataBase::GetInstance()->GetPlayerDesc(m_strName);
    m_fMaxHP = Desc.MaxHP;
    m_fAttackPower = Desc.Attack;
    m_fDefense = Desc.Defend;
    m_tEnergy.fSpecialEnergy = Desc.SpecialAttack;
    Set_EvadeMax(3);

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

HRESULT CJaneDoe::Initialize_Weapon()
{
    ATTACK_COLLIDER_DESC HandL_WeaponDesc;
    HandL_WeaponDesc.eColliderType = COLLIDER_TYPE::BOX;
    HandL_WeaponDesc.pOwnerAnimator = Get_Component<CAnimator3D>();
    HandL_WeaponDesc.tagBone = "Ctr_L_HandWpn_F";
    HandL_WeaponDesc.tagName = "HandWeapon_L";
    HandL_WeaponDesc.vSize = { 0.6f, 0.3f, 0.3f };
    HandL_WeaponDesc.vCenter = { 0.2f, 0.f, 0.f };
    if (FAILED(Attach_AttackCollider(&HandL_WeaponDesc)))
        return E_FAIL;

    ATTACK_COLLIDER_DESC HandR_WeaponDesc;
    HandR_WeaponDesc.eColliderType = COLLIDER_TYPE::BOX;
    HandR_WeaponDesc.pOwnerAnimator = Get_Component<CAnimator3D>();
    HandR_WeaponDesc.tagBone = "Ctr_R_HandWpn_F";
    HandR_WeaponDesc.tagName = "HandWeapon_R";
    HandR_WeaponDesc.vSize = { 0.6f, 0.3f, 0.3f };
    HandR_WeaponDesc.vCenter = { 0.2f, 0.f, 0.f };
    if (FAILED(Attach_AttackCollider(&HandR_WeaponDesc)))
        return E_FAIL;

    ATTACK_COLLIDER_DESC BootsL_WeaponDesc;
    BootsL_WeaponDesc.eColliderType = COLLIDER_TYPE::SPHERE;
    BootsL_WeaponDesc.pOwnerAnimator = Get_Component<CAnimator3D>();
    BootsL_WeaponDesc.tagBone = "Ctr_L_BootsWpn_01";
    BootsL_WeaponDesc.tagName = "FootWeapon_L";
    BootsL_WeaponDesc.vSize = { 0.5f, 0.0f, 0.0f };
    BootsL_WeaponDesc.vCenter = { 0.f, 0.f, 0.f };
    if (FAILED(Attach_AttackCollider(&BootsL_WeaponDesc)))
        return E_FAIL;

    ATTACK_COLLIDER_DESC BootsR_WeaponDesc;
    BootsR_WeaponDesc.eColliderType = COLLIDER_TYPE::SPHERE;
    BootsR_WeaponDesc.pOwnerAnimator = Get_Component<CAnimator3D>();
    BootsR_WeaponDesc.tagBone = "Ctr_R_BootsWpn_01";
    BootsR_WeaponDesc.tagName = "FootWeapon_R";
    BootsR_WeaponDesc.vSize = { 0.5f, 0.f, 0.f };
    BootsR_WeaponDesc.vCenter = { 0.f, 0.f, 0.f };
    if (FAILED(Attach_AttackCollider(&BootsR_WeaponDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CJaneDoe::Initialize_Effects()
{
    auto pObjectContainer = Get_Component<CObjectContainer>();

    // Normal Slash
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_normal1_slash.json")
            .Build("JaneDoe_Normal_Slash0");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_normal1_slash.json")
            .Build("JaneDoe_Normal_Slash1");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_normal1_slash.json")
            .Build("JaneDoe_Normal_Slash2");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_normal1_slash.json")
            .Build("JaneDoe_Normal_Slash3");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_normal1_slash.json")
            .Build("JaneDoe_Normal_Slash4");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }

    // Cross Slash
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_normal2_slash.json")
            .Build("JaneDoe_Cross_Slash");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }

    // Wide Slash
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_normal3_slash.json")
            .Build("JaneDoe_Wide_Slash");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect, false);
    }

    // Sting
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_normal1_sting.json")
            .Build("JaneDoe_Sting");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect, false);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_normal2_sting.json")
            .Build("JaneDoe_Sting2");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }

    // Ex Slash
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_ex_slash.json")
            .Build("JaneDoe_Ex_Slash0");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_ex_slash.json")
            .Build("JaneDoe_Ex_Slash1");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_ex_slash.json")
            .Build("JaneDoe_Ex_Slash2");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }
    {
        auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("janedoe_ex_slash.json")
            .Build("JaneDoe_Ex_Slash3");
        pEffect->Stop();
        pObjectContainer->Add_Child(pEffect);
    }

    return S_OK;
}

void CJaneDoe::Update_States()
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

void CJaneDoe::Process_AttackInput(const string& strCurrentState)
{
    if (strCurrentState == "Idle")
    {
        m_pStateMachine->Set_Int("AttackEntryMode", 0);
        m_pStateMachine->Set_Trigger("Attack");
    }
    else if (strCurrentState == "Move")
    {
        CJaneDoeState_Move* pMove = static_cast<CJaneDoeState_Move*>(
            m_pStateMachine->Get_CurrentState());
        if (!pMove || !pMove->Get_SubStateMachine())
            return;

        string strMoveType = pMove->Get_SubStateMachine()->Get_CurrentStateName();

        if (strMoveType == "Walk")
            m_pStateMachine->Set_Int("AttackEntryMode", 0);
        else if (strMoveType == "Run")
        {
            IHState<CJaneDoe>* pRun = dynamic_cast<IHState<CJaneDoe>*>(
                pMove->Get_SubStateMachine()->Get_CurrentState());
            if (pRun && pRun->Get_SubStateMachine())
            {
                string strRunTag = pRun->Get_SubStateMachine()->Get_CurrentStateName();
                if (strRunTag == "End") m_pStateMachine->Set_Int("AttackEntryMode", 0);
                else m_pStateMachine->Set_Int("AttackEntryMode", 1);
            }
            else return;
        }
        else return;

        m_pStateMachine->Set_Trigger("Attack");
    }
    else if (strCurrentState == "Attack")
    {
        CJaneDoeState_Attack* pAttack = static_cast<CJaneDoeState_Attack*>(
            m_pStateMachine->Get_CurrentState());
        if (!pAttack || !pAttack->Get_SubStateMachine())
            return;

        if (pAttack->Get_SubStateMachine()->Get_CurrentStateName() != "NormalAttack")
            return;

        CJaneDoeState_NormalAttack* pNormal = static_cast<CJaneDoeState_NormalAttack*>(
            pAttack->Get_SubStateMachine()->Get_State("NormalAttack"));
        if (pNormal && pNormal->Get_SubStateMachine())
            pNormal->Get_SubStateMachine()->Set_Trigger("NextCombo");
    }
    else if (strCurrentState == "SwitchIn")
    {
        CJaneDoeState_SwitchIn* pSwitchIn = static_cast<CJaneDoeState_SwitchIn*>(
            m_pStateMachine->Get_CurrentState());
        if (!pSwitchIn || !pSwitchIn->Get_SubStateMachine())
            return;

        string strSwitchType = pSwitchIn->Get_SubStateMachine()->Get_CurrentStateName();
        if (strSwitchType == "SwitchInParryAid")
        {
            CJaneDoeState_SwitchInParryAid* pParryAid = static_cast<CJaneDoeState_SwitchInParryAid*>(
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

void CJaneDoe::Process_EndState(const string& strCurrentState)
{
    if (strCurrentState == "Move")
    {
        CJaneDoeState_Move* pMove = static_cast<CJaneDoeState_Move*>(
            m_pStateMachine->Get_CurrentState());
        if (!pMove) return;

        IHState<CJaneDoe>* pMoveType = dynamic_cast<IHState<CJaneDoe>*>(
            pMove->Get_SubStateMachine()->Get_CurrentState());
        if (pMoveType && pMoveType->Is_EndState())
        {
            IBaseState<CJaneDoe>* pEnd = pMoveType->Get_SubStateMachine()->Get_CurrentState();
            if (m_bIsAttack || m_bIsEvade) return;
            if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
                m_pStateMachine->Set_Trigger("ToIdle");
        }
    }
    else if (strCurrentState == "Attack")
    {
        CJaneDoeState_Attack* pAttack = static_cast<CJaneDoeState_Attack*>(
            m_pStateMachine->Get_CurrentState());
        if (!pAttack) return;

        IHState<CJaneDoe>* pAttackType = dynamic_cast<IHState<CJaneDoe>*>(
            pAttack->Get_SubStateMachine()->Get_CurrentState());
        if (pAttackType && pAttackType->Is_EndState())
        {
            IBaseState<CJaneDoe>* pEnd = pAttackType->Get_SubStateMachine()->Get_CurrentState();
            if (m_bIsEvade) return;
            if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
                m_pStateMachine->Set_Trigger("ToIdle");
        }
    }
    else if (strCurrentState == "SwitchIn")
    {
        CJaneDoeState_SwitchIn* pSwitchIn = static_cast<CJaneDoeState_SwitchIn*>(
            m_pStateMachine->Get_CurrentState());
        if (!pSwitchIn) return;

        IHState<CJaneDoe>* pSwitchInType = dynamic_cast<IHState<CJaneDoe>*>(
            pSwitchIn->Get_SubStateMachine()->Get_CurrentState());
        if (pSwitchInType && pSwitchInType->Is_EndState())
        {
            IBaseState<CJaneDoe>* pEnd = pSwitchInType->Get_SubStateMachine()->Get_CurrentState();
            if (m_bIsEvade) return;
            if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
                m_pStateMachine->Set_Trigger("ToIdle");
        }
    }
    else if (strCurrentState == "Hit")
    {
        CJaneDoeState_Hit* pHit = static_cast<CJaneDoeState_Hit*>(
            m_pStateMachine->Get_CurrentState());
        if (!pHit || !pHit->Get_SubStateMachine()) return;

        IBaseState<CJaneDoe>* pHitType = pHit->Get_SubStateMachine()->Get_CurrentState();
        if (pHitType && pHitType->Get_AnimProgress() > 0.3f)
        {
            if (m_bIsEvade) return;
            if (Is_Input() || pHitType->Is_AnimEnd())
                m_pStateMachine->Set_Trigger("ToIdle");
        }
    }
}

HRESULT CJaneDoe::Add_PassionMotionBlur()
{
    auto Model = Get_Component<CSkeletalModel>();
    _uint size = sizeof(_float4x4) * m_pAnimator->Get_BoneMatrices(CAnimator3D::BoneSpace::COMBINED).size();
    m_vRimLightColor = _float3(1.f, 0.1f, 0.f);
    m_fRimLightPower = 3.f;

    for (_int k = m_BoneMatrices.size() - 1; k >= 0; --k)
    {
        _float t = (_float)k / (_float)(m_BoneMatrices.size() - 1);
        _float4 vColor;
        vColor.x = 0.3f + (0.7f * t);
        vColor.y = 0.0f + (0.15f * t);
        vColor.z = 0.0f;
        vColor.w = 0.08f + (0.9f * t);

        for (_int i = 0; i < Model->Get_MeshCount(); ++i)
        {
            if (Model->isDrawable(i) == false) continue;
            MOTIONBLUR_COMMAND Command =
            {
                Get_Component<CMaterial>()->Get_Shader(Model->Get_MaterialIndex(i)),
                &m_WorldMatrices[k],
                m_BoneMatrices[k][i],
                "float4x4[]",
                vColor,
                size,
                i,
                [this](ID3D11DeviceContext* pContext, _uint index) {Render_PassionMotionBlur(pContext, index); }
            };
            RenderSystem()->Add_MotionBlurCommand(Command);
        }
    }
    return S_OK;
}

HRESULT CJaneDoe::Render_PassionMotionBlur(ID3D11DeviceContext* pContext, _uint idx)
{
    auto RenderSys = RenderSystem()->GetRenderer(RENDERER_TYPE::SKINNED);
    auto Model = Get_Component<CSkeletalModel>();
    auto Material = Get_Component<CMaterial>();
    _int Index = Model->Get_MaterialIndex(idx);
    auto Shader = Material->Get_Shader(Index);
    ID3D11InputLayout* pLayout;
    RenderSys->Get_InputLayout(
        Model,
        Shader,
        idx,
        "MotionBlur",
        &pLayout
    );

    pContext->IASetInputLayout(pLayout);
    Shader->Apply("MotionBlur", pContext);
    Model->Draw(pContext, idx);

    return S_OK;
}

CJaneDoe* CJaneDoe::Create()
{
    CJaneDoe* Instance = new CJaneDoe();
    if (FAILED(Instance->Initialize_Prototype()))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

CGameObject* CJaneDoe::Clone(INIT_DESC* pArg)
{
    CJaneDoe* Instance = new CJaneDoe(*this);
    if (FAILED(Instance->Initialize(pArg)))
    {
        Safe_Release(Instance);
        return nullptr;
    }
    return Instance;
}

void CJaneDoe::Free()
{
    Safe_Release(m_pStateMachine);
    __super::Free();
}