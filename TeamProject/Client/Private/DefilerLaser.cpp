#include "pch.h"
#include "DefilerLaser.h"
#include "EffectContainer.h"
#include "BattleSystem.h"
#include "GameInstance.h"

//component
#include "ObjectContainer.h"
#include "RigidBody.h"
#include "Collider.h"
#include "BoneFollower.h"

#include "Child.h"
#include "Defiler.h"

CDefilerLaser::CDefilerLaser()
    :CEnemy()
{
}

CDefilerLaser::CDefilerLaser(const CDefilerLaser& rhg)
    :CEnemy(rhg)
{
}

HRESULT CDefilerLaser::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CRigidBody>();
    Add_Component<CCollider>();
    Add_Component<CBoneFollower>();
    return S_OK;
}

HRESULT CDefilerLaser::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    auto pCollider = Get_Component<CCollider>();
    pCollider->Set_Trigger(false);
    pCollider->Set_CollisionGroup(COLLISION_GROUP::MONSTER);
    pCollider->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER_ATTACK));
    pCollider->Set_Size(_float3(2.f, 2.f, 2.f));
    pCollider->Set_CompActive(false);

    auto pRigidBody = Get_Component<CRigidBody>();
    pRigidBody->Set_Kinematic(true);
    
    _smatrix offsetMatrix = _smatrix::Identity;
    offsetMatrix.Translation(_vector3(2.f, 0.f, 0.f));
    auto pBoneFollwer = Get_Component<CBoneFollower>();
    pBoneFollwer->Set_Offset(offsetMatrix);

    {
        BATTLE_COLLIDER_DESC BladeDesc{};

        BladeDesc.tagName = "Laser_Attack";
        BladeDesc.isAttachBone = false;
        BladeDesc.tagBone = "";
        BladeDesc.pOwnerAnimator3D = nullptr;
        BladeDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
        BladeDesc.vAttackSize = _float3{ 2.5f,2.5f,2.5f };

        if (FAILED(AttachBattleColliderObject(&BladeDesc, false)))
            return E_FAIL;
    }

    if (FAILED(Initialize_Effects()))
        return E_FAIL;

    return S_OK;
}

void CDefilerLaser::Awake()
{
}

void CDefilerLaser::Priority_Update(_float dt)
{
}

void CDefilerLaser::Update(_float dt)
{
    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (m_IsPendingActive)
    {
        Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);
        Apply_PendingActive();
    }

    if (m_IsActive)
    {
        m_fElapsedTime += dt;
        if (m_fElapsedTime >= m_fDuration)
        {
            m_IsPendingActive = false;
            m_IsActive = false;
            m_fElapsedTime = 0.f;
            m_isOnAttack = false;

            Get_Component<CCollider>()->Set_CompActive(false);
            SetBattleColliderObject("Laser_Attack", CEnemy::BATTLE_COLTYPE::ATTACK, false);

            auto pLaser = Get_Component<CObjectContainer>()->Find_ObjectByName("Defiler_Laser");
            if(pLaser)
                static_cast<CEffectContainer*>(pLaser)->Stop();
        }
    }
}

void CDefilerLaser::Late_Update(_float dt)
{
}

void CDefilerLaser::Render_GUI()
{
    __super::Render_GUI();
}

HRESULT CDefilerLaser::Initialize_Effects()
{
    auto pObjectContainer = Get_Component<CObjectContainer>();

    /* Shot Point */
    {
        auto pLaserStart = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("defiler_laser_shot_normal0.json")
            .Build("Defiler_Normal_Shot");

        if (pLaserStart)
        {
            pLaserStart->Stop();
            pObjectContainer->Add_Child(pLaserStart, false);
        }
    }

    {
        auto pLaserStart = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("defiler_laser_shot_strong0.json")
            .Build("Defiler_Strong_Shot");

        if (pLaserStart)
        {
            pLaserStart->Stop();
            pObjectContainer->Add_Child(pLaserStart, false);
        }
    }

    /* Laser */
    {
        auto pLaser = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("defiler_laser.json")
            .Build("Defiler_Laser");

        if (pLaser)
        {
            pLaser->Stop();
            pObjectContainer->Add_Child(pLaser,false);
        }
    }

    /* Laser Hit Ground Point */
    {
        auto pLaserHitPoint = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
            .Asset("defiler_laser_hit_ground0.json")
            .Build("Defiler_Laser_HitGround");

        if (pLaserHitPoint)
        {
            pLaserHitPoint->Stop();
            pObjectContainer->Add_Child(pLaserHitPoint, false);
        }
    }

    return S_OK;
}

void CDefilerLaser::Apply_PendingActive()
{
    m_IsPendingActive = false;
    m_IsActive = true;
    m_isOnAttack = true;
    Get_Component<CCollider>()->Set_CompActive(true);

    HitDesc		HitDesc = {};
    HitDesc.eHitType = HIT_TYPE::ONCE;
    HitDesc.eDamageType = DAMAGE_TYPE::NORMAL;
    HitDesc.fDamage = 10.f;
    HitDesc.fInterval = 0.f;
    HitDesc.iMaxCount = 1;
    SetBattleColliderObject("Laser_Attack", CEnemy::BATTLE_COLTYPE::ATTACK, true, HitDesc);

    SetUp_Effect();
    SetUp_Collider();
}

void CDefilerLaser::SetUp_Effect()
{
    m_vStartPoint = _vector3(m_pTransform->Get_WorldPos());

    _vector3 vCurrPosition = m_vStartPoint;
    _vector3 vTargetPosition = BattleSystem()->GetCurCharacterHandle().Get()->Get_Component<CTransform>()->Get_WorldPos();
    if (auto child = Get_Component<CChild>()) {
        if (auto parent = child->Get_Parent()) {
          auto board =   dynamic_cast<CDefiler*>(parent)->GetBlackBoard();
          vTargetPosition = board.vTargetPos;
        }
    }
    vTargetPosition.y += 1.f;
    _vector3 vTargetDir = vTargetPosition - vCurrPosition;

    if (vTargetDir.Length() <= 0.01f)
        vTargetDir = _vector3(0.f, 0.f, 1.f);
    else
        vTargetDir.Normalize();

    PHYSICS_RAY rayDesc{};
    PHYSICS_RAY_HIT output{};
    rayDesc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON) + ENUM(COLLISION_GROUP::GROUND) + ENUM(COLLISION_GROUP::NAP);
    rayDesc.vOrigin = vCurrPosition;
    rayDesc.vDirection = vTargetDir;
    rayDesc.fMaxDistance = 200.f;

    if (PhysicsSystem()->Raycast(rayDesc, output))
        m_vEndPoint = output.vPoint;
    else
        m_vEndPoint = vCurrPosition + 200.f * vTargetDir;

    auto pEffectContainer = Get_Component<CObjectContainer>()->Find_ObjectByName("Defiler_Laser");
    CEffectContainer::EFFECT_CONTAINER_CONTEXT& context = static_cast<CEffectContainer*>(pEffectContainer)->GetEffectContext();
    
    context.vLinePoint0 = m_vStartPoint;
    context.vLinePoint1 = m_vEndPoint;

    m_pTransform->Set_Look(vTargetDir);

    auto pObjectContainer = Get_Component<CObjectContainer>();
    
    switch (m_eType)
    {
    case CDefilerLaser::LASER_TYPE::NORMAL:
    {
        auto pLaserStart = pObjectContainer->Find_ObjectByName("Defiler_Normal_Shot");
        if (pLaserStart)
        {
            auto pLaserTransform = pLaserStart->Get_Component<CTransform>();
            pLaserTransform->Set_WorldPos(_vector3(m_vStartPoint));
            pLaserTransform->Set_Look(_vector3(vTargetDir));

            static_cast<CEffectContainer*>(pLaserStart)->Play();
        }
    }break;
    case CDefilerLaser::LASER_TYPE::STRONG:
    {
        auto pLaserStart = pObjectContainer->Find_ObjectByName("Defiler_Strong_Shot");
        if (pLaserStart)
        {
            auto pLaserTransform = pLaserStart->Get_Component<CTransform>();
            pLaserTransform->Set_WorldPos(_vector3(m_vStartPoint));
            pLaserTransform->Set_Look(_vector3(vTargetDir));

            static_cast<CEffectContainer*>(pLaserStart)->Play();
        }
    }break;
    default:
        break;
    }

    auto pLaser = pObjectContainer->Find_ObjectByName("Defiler_Laser");
    if (pLaser)
    {
        auto pLaserTransform = pLaser->Get_Component<CTransform>();
        pLaserTransform->Set_WorldPos(_vector3(m_vStartPoint));
        pLaserTransform->Set_Look(_vector3(vTargetDir));

        static_cast<CEffectContainer*>(pLaser)->Play();
    }

    auto pLaserHitPoint = pObjectContainer->Find_ObjectByName("Defiler_Laser_HitGround");
    if (pLaserHitPoint)
    {
        pLaserHitPoint->Get_Component<CTransform>()->Set_Pos(m_vEndPoint);
        static_cast<CEffectContainer*>(pLaserHitPoint)->Play();
    }
}

void CDefilerLaser::SetUp_Collider()
{
    auto pAttackCollider = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser_Attack_AttackCollider")->Get_Component<CCollider>();
    auto pCollider = Get_Component<CCollider>();
    auto pRigidBody = Get_Component<CRigidBody>();

    _vector3 vDir = _vector3(m_vEndPoint)-_vector3(m_vStartPoint);
    _float fLength = vDir.Length();
    vDir.Normalize();

    m_pTransform->Set_Look(vDir);

    pCollider->Set_Center(_float3(0.f, 0.f, fLength * 0.5f));
    pCollider->Set_Size(_float3(1.f, 1.f, fLength));

    pAttackCollider->Set_Center(_float3(0.f, 0.f, fLength * 0.5f));
    pAttackCollider->Set_Size(_float3(2.f, 2.f, fLength));
}

void CDefilerLaser::Set_ActiveLaser(_bool active, LASER_TYPE eType)
{
    m_IsPendingActive = active;
    m_eType = eType;
}

CDefilerLaser* CDefilerLaser::Create()
{
    CDefilerLaser* instance = new CDefilerLaser();

    if (FAILED(instance->Initialize_Prototype()))
    {
        Safe_Release(instance);
        MSG_BOX("Failed to create : CDefilerLaser");
    }

    return instance;
}

CGameObject* CDefilerLaser::Clone(INIT_DESC* pArg)
{
    CDefilerLaser* instance = new CDefilerLaser(*this);

    if (FAILED(instance->Initialize(pArg)))
    {
        Safe_Release(instance);
        MSG_BOX("Failed to clone : CDefilerLaser");
    }

    return instance;
}

void CDefilerLaser::Free()
{
    __super::Free();
}
