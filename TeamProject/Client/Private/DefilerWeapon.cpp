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

	Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER) |
		ENUM(COLLISION_GROUP::PLAYER_ATTACK));
	Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::MONSTER);
	Get_Component<CCollider>()->Set_Trigger(true);
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
	return S_OK;
}

void CDefilerWeapon::Awake()
{
	
}

void CDefilerWeapon::Priority_Update(_float dt)
{
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

		if (m_isEnd && m_Dissolve.isComplete())
			ObjectManager()->Remove_Object(this);

		if (speedXZ < stopSpeed)
		{
			m_slideVelXZ = { 0.f, 0.f, 0.f };
		}

		return;
	}
	else if(m_isSliding && m_isFinalThrow) {
		SummonAxe();
		ObjectManager()->Remove_Object(this);
		return;
	}

	float t = Math::ApplyEase(EaseType::OutExpo, m_ElapsedTime);
	t = clamp(t, 0.f, 1.f);

	m_vVelocity = m_vVelocity.Lerp(m_vVelocity, m_vTargetVelocity, t);
	m_pTransform->Translate(m_vVelocity * dt);

	Get_Component<CCollider>()->Update(dt);
	Update_Dissolve(dt);
}
void CDefilerWeapon::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CDefilerWeapon::Render_GUI()
{
	__super::Render_GUI();
}

void CDefilerWeapon::OnPooledAcquire(INIT_DESC* pArg)
{
	auto desc = static_cast<DefilerWeaponDesc*>(pArg);
	__super::Initialize(desc);
	Reset_Value(desc);

	Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER) |
		ENUM(COLLISION_GROUP::PLAYER_ATTACK));
	Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::MONSTER);
	Get_Component<CCollider>()->Set_Trigger(true);
	Get_Component<CRigidBody>()->Set_Kinematic(true);
}

void CDefilerWeapon::OnPooledRelease()
{
	m_isOnAttack = false;
	Get_Component<CCollider>()->Set_CompActive(false);
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
	if (pCollidable)
		return;

	else {
		auto pEnemy = dynamic_cast<CCharacter*>(pOther);
		if (nullptr != pEnemy)
		{
			pEnemy->Take_Damage(DAMAGE_TYPE::NORMAL, 10);
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
	MonsterCCT.iCollisionMask =  ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
	MonsterCCT.bAutoFit = false;
	MonsterCCT.fHeight = .3f;
	MonsterCCT.fRadius = 2.f;
	MonsterCCT.vPos = pos;
	MonsterCCT.vPos.y += MonsterCCT.fHeight;

	auto pBlade =
		Builder::Create_Object({ "Zero_Level","Proto_GameObject_DefilerAxe" })
		.FromPool()
		.Add_ObjDesc(desc)
		.CharacterController(MonsterCCT)
		.Build("DefilerAxe");

	ObjectManager()->Add_Object(pBlade, { levelKey ,"Enemy_Layer" });
	BattleSystem()->EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, pBlade->Get_Handle());
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