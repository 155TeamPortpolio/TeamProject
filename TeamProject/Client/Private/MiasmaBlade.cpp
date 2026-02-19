#include "pch.h"
#include "MiasmaBlade.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "StaticModel.h"
#include "Material.h"
#include "Collider.h"
#include "RigidBody.h"
#include "ObjectContainer.h"

#include "Helper_Func.h"
#include "Character.h"
#include "Defiler.h"
#include "EffectContainer.h"

CMiasmaBlade::CMiasmaBlade()
	: CEnemy()
{
}

CMiasmaBlade::CMiasmaBlade(const CMiasmaBlade& rhs)
	:CEnemy(rhs)
{
}

HRESULT CMiasmaBlade::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	//Add_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "Default.model");
	//Add_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Default.mat");
	Add_Component<CRigidBody>();
	Add_Component<CCollider>();
	return S_OK;
}

HRESULT CMiasmaBlade::Initialize(INIT_DESC* pArg)
{
	auto desc = static_cast<BladeDesc*>(pArg);
	__super::Initialize(desc);
	m_pOwner = desc->pOwner;
	m_isOnAttack = true;
	m_isParryEnable = true;
	isParried = false;

	Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER) |ENUM(COLLISION_GROUP::PLAYER_ATTACK));
	Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::MONSTER);

	Get_Component<CCollider>()->Set_Trigger(false);
	Get_Component<CRigidBody>()->Set_Kinematic(false);
	Get_Component<CRigidBody>()->Set_Gravity(false);
	Get_Component<CRigidBody>()->Set_CCD(true);

	m_pTransform->LookAt(_vector3(desc->vTargetPos));
	m_vVelocity = { 0,0,0 };
	m_vTargetVelocity = m_pTransform->Dir(STATE::LOOK) * m_fMoveSpeed;
	m_ElapsedTime = 0;

	Initialize_Effects(desc->iCount);
	
	{
		BATTLE_COLLIDER_DESC BladeDesc{};
	
		BladeDesc.tagName = "HEl";
		BladeDesc.isAttachBone = false;
		BladeDesc.tagBone = "";
		BladeDesc.pOwnerAnimator3D = nullptr;
		BladeDesc.eAttackColliderType = COLLIDER_TYPE::SPHERE;
		BladeDesc.vAttackSize = _float3{ 2.5f,2.5f,2.5f };
	
		if (FAILED(AttachBattleColliderObject(&BladeDesc,false)))
			return E_FAIL;
	}

	HitDesc		HitDesc = {};
	HitDesc.eHitType = HIT_TYPE::ONCE;
	HitDesc.eDamageType = DAMAGE_TYPE::NORMAL;
	HitDesc.fDamage = 10.f;
	HitDesc.fInterval = 0.f;
	HitDesc.iMaxCount = 1;
	SetBattleColliderObject("HEl", CEnemy::BATTLE_COLTYPE::ATTACK, true, HitDesc);

	return S_OK;
}

void CMiasmaBlade::Awake()
{
}

void CMiasmaBlade::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CMiasmaBlade::Update(_float dt)
{
	m_ElapsedTime += dt;

	m_vVelocity = m_vVelocity.Lerp(m_vVelocity,m_vTargetVelocity, Math::ApplyEase(EaseType::OutExpo, m_ElapsedTime));

	if (isParried) {
		_vector3 target_Pos = m_pOwner->Get_BipedPos();
		_float4 pos =  Get_Position();
		_vector3 ownPos = { pos.x,pos.y,pos.z};

		if ((target_Pos - ownPos).Length() < 3.f){
			m_pOwner->TakeDamage(DAMAGE_TYPE::NORMAL, 10);
			BattleSystem()->ExitBattleObject(BATTLE_OBJ_TYPE::MONSTER, Get_Handle());
			ObjectManager()->Remove_Object(this);
		}
	}
	Get_Component<CRigidBody>()->Set_Velocity(m_vVelocity);
	Get_Component<CCollider>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);

	if (m_ElapsedTime > 10.f) {
		BattleSystem()->ExitBattleObject(BATTLE_OBJ_TYPE::MONSTER, Get_Handle());
		ObjectManager()->Remove_Object(this);
	}
}

void CMiasmaBlade::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CMiasmaBlade::Render_GUI()
{
    __super::Render_GUI();
}

void CMiasmaBlade::Parried()
{
	if (m_pOwner) {
		isParried = true;
		_vector3 pos = m_pOwner->Get_BipedPos();
		m_pTransform->LookAt(_vector3(pos));
		Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::MONSTER));
		Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::PLAYER_ATTACK);
		m_vVelocity = {0,0,0};
		m_ElapsedTime = 1;
		m_vTargetVelocity = m_pTransform->Dir(STATE::LOOK) * m_fMoveSpeed;
		Get_Component<CRigidBody>()->Set_Velocity(m_vVelocity);
	}
}

void CMiasmaBlade::Initialize_Effects(_uint count)
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_miasma.json")
			.Build("Defiler_Miasma");

		auto pEffectTransform = pEffect->Get_Component<CTransform>();
		switch (count)
		{
		case 0:
			pEffectTransform->Rotate(_vector3(0.f, 0.f, 0.7747f));
			break;
		case 1:
			pEffectTransform->Rotate(_vector3(0.f, 0.f, -0.7875f));
			break;
		case 2:
			pEffectTransform->Rotate(_vector3(0.f, 0.f, 1.6f));
			break;
		default:
			break;
		}

		pObjectContainer->Add_Child(pEffect);
	}
}

CMiasmaBlade* CMiasmaBlade::Create()
{
	CMiasmaBlade* instance = new CMiasmaBlade();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefiler");
	}

	return instance;
}

CGameObject* CMiasmaBlade::Clone(INIT_DESC* pArg)
{
	CMiasmaBlade* instance = new CMiasmaBlade(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefiler");
	}

	return instance;
}

void CMiasmaBlade::Free()
{
	__super::Free();
}
