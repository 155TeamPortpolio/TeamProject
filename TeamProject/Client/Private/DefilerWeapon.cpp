#include "pch.h"
#include "DefilerWeapon.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Collider.h"
#include "RigidBody.h"
#include "ObjectContainer.h"

#include "Helper_Func.h"
#include "Character.h"
#include "Defiler.h"
#include "Texture.h"
#include "AudioSource.h"
#include "DefilerAxe.h"
#include "EffectContainer.h"
#include "CamDirector.h"

CDefilerWeapon::CDefilerWeapon()
	: CEnemy()
{
}

CDefilerWeapon::CDefilerWeapon(const CDefilerWeapon& rhs)
	:CEnemy(rhs)
{
}

HRESULT CDefilerWeapon::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CSkeletalModel>()->Link_Model("Zero_Level", "Defiler_Weapon.model");
	Add_Component<CMaterial>()->Link_Material("Zero_Level", "Defiler_Weapon.mat");
	Add_Component<CCollider>();
	Add_Component<CRigidBody>();
	Add_Component<CAudioSource>();

	return S_OK;
}

HRESULT CDefilerWeapon::Initialize(INIT_DESC* pArg)
{
	auto desc = static_cast<DefilerWeaponDesc*>(pArg);
	__super::Initialize(desc);
	Reset_Value(desc);

	Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::GROUND) |ENUM(COLLISION_GROUP::PLAYER_ATTACK));
	Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::MONSTER);
	Get_Component<CCollider>()->Set_Size(m_isFinalThrow? _vector3{3.f, 3.f, 3.f} : _vector3{2.f,2.f,2.f});
	Get_Component<CCollider>()->Set_Trigger(false);
	Get_Component<CRigidBody>()->Set_Kinematic(true);
	Get_Component<CAudioSource>()->SoundFolder("Zero_Level", "../Bin/Resources/Zero/Enemy/Defiler_Isolde/Sound/");

	m_vRimLightColor = _float3(0.378, 0.029, 0.070);
	m_fRimLightPower = 4.f;
	m_fDissolveTilling = 6.f;

	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	auto dissolveTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Dissolve.png");

	for (const auto& instance : materialInstances)
	{
		instance->Set_Param("NoiseTexture", { dissolveTexture->Get_SRV(),"Texture2D",0 });
		instance->Set_Param("vRimLightColor", { &m_vRimLightColor,"float3",sizeof(_float3) });
		instance->Set_Param("fRimLightPower", { &m_fRimLightPower,"float",sizeof(_float) });
		instance->Set_Param("fDissolveProgress", { &m_fDissolveProgress,"float",sizeof(_float) });
		instance->Set_Param("fDissolveTiling", { &m_fDissolveTilling,"float",sizeof(_float) });
	}
	m_isOnAttack = true;
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

	if (FAILED(Initialize_Effects()))
		return E_FAIL;

	return S_OK;
}

void CDefilerWeapon::Awake()
{
	
}

void CDefilerWeapon::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}
void CDefilerWeapon::Update(_float dt)
{
	m_ElapsedTime += dt;

	const _vector3 nowPos = Get_WorldPos();
	const bool hitGround = m_vTargetPos.y + 0.5f >= nowPos.y;

	if (hitGround && !m_isSliding)
	{
		m_isSliding = true;
		m_groundY = nowPos.y;
		m_slideVelXZ = m_vVelocity;
		m_slideVelXZ.y = 0.f;
		Get_Component<CAudioSource>()->Slot("DefilerWeaponGround.wav").Volume(0.5f).Play();

		Stop_Effect("Defiler_Throw_Axe_Trail");

		_vector3 vWorldPosition = m_pTransform->Get_WorldPos();
		vWorldPosition.y += 0.1f;
		if (m_isFinalThrow)
		{
			auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
				.Asset("defiler_throw_axe_hit_ground1.json")
				.Position(vWorldPosition)
				.Build("Defiler_Throw_Axe_HitGround_Strong");

			ObjectManager()->Add_Object(pEffect, { Get_Level(),"Enemy_Effect_Layer" });
		}	
		else
		{
			auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
				.Asset("defiler_throw_axe_hit_ground0.json")
				.Position(vWorldPosition)
				.Build("Defiler_Throw_Axe_HitGround_Normal");

			ObjectManager()->Add_Object(pEffect, { Get_Level(),"Enemy_Effect_Layer" });
		}

	}

	if (m_isSliding&& !m_isFinalThrow)
	{
		const float damping = expf(-20.f * dt);
		m_slideVelXZ *= damping;
		_vector3 move = m_slideVelXZ * dt;
		move.y = 0.f;

		_vector3 newPos = nowPos;
		newPos += move;
		newPos.y = m_groundY; // Y °íÁ¤
		m_pTransform->Set_Pos(newPos);

		Get_Component<CCollider>()->Update(dt);
		Update_Dissolve(dt);

		const float stopSpeed = 0.15f;
		const float speedXZ = _vector3(m_slideVelXZ.x, 0.f, m_slideVelXZ.z).Length();
		if (m_ElapsedTime > 0.3f && !m_isEnd)
		{
			m_Dissolve.DisAppear(0.5f);
			m_isEnd = true;
		}

		if (m_isEnd && m_Dissolve.isComplete()) {
			ObjectManager()->Remove_Object(this);
			CameraManager()->AddImpact(ENUM(CamShakeType::EarthquakeShort), ENUM(CamZoomType::EarthquakeShort));
		}

		if (speedXZ < stopSpeed)
		{
			m_slideVelXZ = { 0.f, 0.f, 0.f };
		}

		return;
	}
	else if(m_isSliding && m_isFinalThrow) {
		SummonAxe();
		ObjectManager()->Remove_Object(this);
		BattleSystem()->ExitBattleObject(BATTLE_OBJ_TYPE::MONSTER, Get_Handle());
		CameraManager()->AddImpact(ENUM(CamShakeType::EarthquakeShort), ENUM(CamZoomType::EarthquakeShort));
		return;
	}

	float t = Math::ApplyEase(EaseType::OutExpo, m_ElapsedTime);
	t = clamp(t, 0.f, 1.f);

	m_vVelocity = m_vVelocity.Lerp(m_vVelocity, m_vTargetVelocity, t);
	m_pTransform->Translate(m_vVelocity * dt);
	Get_Component<CRigidBody>()->Set_GlobalPos(m_pTransform->Get_WorldPos(), m_pTransform->Get_QuaternionRotate());
	Get_Component<CCollider>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);
	Update_Dissolve(dt);
}
void CDefilerWeapon::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CDefilerWeapon::Render_GUI()
{
	__super::Render_GUI();
}


CDefilerWeapon* CDefilerWeapon::Create()
{
	CDefilerWeapon* instance = new CDefilerWeapon();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefiler");
	}

	return instance;
}

CGameObject* CDefilerWeapon::Clone(INIT_DESC* pArg)
{
	CDefilerWeapon* instance = new CDefilerWeapon(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefiler");
	}

	return instance;
}

void CDefilerWeapon::Free()
{
	__super::Free();
}

void CDefilerWeapon::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (!pCollidable)
		return;

	else {
		auto pEnemy = dynamic_cast<CCharacter*>(pOther);
		if (nullptr != pEnemy)
		{
			pEnemy->Take_Damage(DAMAGE_TYPE::HARD, 10);
			CameraManager()->AddImpact(1, 0);
		}
	}
}

_bool CDefilerWeapon::Try_Hit(CGameObject* pTarget)
{
	return true;
}

void CDefilerWeapon::Reset_Value(DefilerWeaponDesc* pArg)
{
	m_isFinalThrow = pArg->isFinal;
	m_fMoveSpeed = m_isFinalThrow? 150.f : 120.f;
	m_isOnAttack	= true;
	m_isParryEnable = false;
	m_isEnd			= false;
	m_isSliding		= false;
	m_vTargetPos = _vector3(pArg->vTargetPos);
	_vector3 pos = m_vTargetPos; 
	m_pTransform->LookAt(pos);

	m_vTargetVelocity = m_pTransform->Dir(STATE::LOOK) * m_fMoveSpeed;
	m_vVelocity = { 0,0,0 };
	m_slideVelXZ = { 0.f,0.f,0.f };

	m_ElapsedTime = 0;
	m_groundY = 0.f;

	m_fDissolveProgress = 1.01f;
	m_Dissolve.fDissolveElapsedTime = 0.f;
	m_Dissolve.Appear(0.01f);
}

void CDefilerWeapon::SummonAxe()
{
	const string levelKey = LevelManager()->Get_NowLevelKey();
	_vector3 pos = m_pTransform->Get_Pos();
	CDefilerAxe::DefilerAxeDesc* desc = new CDefilerAxe::DefilerAxeDesc;
	desc->vLook = m_pTransform->Dir(STATE::LOOK);

	CCT_DESC MonsterCCT;
	MonsterCCT.eGroup = COLLISION_GROUP::MONSTER;
	MonsterCCT.iCollisionMask =  ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::GROUND) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
	MonsterCCT.bAutoFit = false;
	MonsterCCT.fHeight = .3f;
	MonsterCCT.fRadius = 2.f;
	MonsterCCT.vPos = pos;
	MonsterCCT.vPos.y += MonsterCCT.fHeight;

	auto pBlade =
		Builder::Create_Object({ "Zero_Level","Proto_GameObject_DefilerAxe" })
		.Add_ObjDesc(desc)
		.CharacterController(MonsterCCT)
		.Build("DefilerAxe");

	ObjectManager()->Add_Object(pBlade, { levelKey ,"Enemy_Layer" });
	BattleSystem()->EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, pBlade->Get_Handle());
}

HRESULT CDefilerWeapon::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	/* Trail */
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_throw_axe_trail.json")
			.Build("Defiler_Throw_Axe_Trail");
		
		pEffect->Play();
		pObjectContainer->Add_Child(pEffect);
	}

	return S_OK;
}

void CDefilerWeapon::Play_Effect(const string& effectTag, _fvector offsetPosition, _fvector offsetQuaternion, _bool syncTransform)
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

void CDefilerWeapon::Stop_Effect(const string& effectTag)
{
	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName(effectTag);
	if (!pEffect)
		return;

	static_cast<CEffectContainer*>(pEffect)->Stop();
}

void CDefilerWeapon::Update_Dissolve(_float dt)
{
	const _float duration = m_Dissolve.fDissolveDuration;
	if (duration <= 0.f)
	{
		m_fDissolveProgress =
			(m_Dissolve.eDissolveState == DefilerDissolve::DISAPPEAR) ? 1.f : 0.f;
		return;
	}

	m_Dissolve.fDissolveElapsedTime =
		min(m_Dissolve.fDissolveElapsedTime + dt, duration);

	_float t = m_Dissolve.fDissolveElapsedTime / duration; // 0..1

	switch (m_Dissolve.eDissolveState)
	{
	case DefilerDissolve::DISAPPEAR: m_fDissolveProgress = t; break;
	case DefilerDissolve::APPEAR:   m_fDissolveProgress = 1.f - t; break;
	default: break;
	}
}