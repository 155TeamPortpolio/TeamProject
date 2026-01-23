#include "pch.h"
#include "CharacterAttackCollider.h"

#include "GameInstance.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"

#include "RigidBody.h"
#include "Collider.h"
#include "BoneFollower.h"
#include "Child.h"

#include "Enemy.h"
#include "Character.h"
// Camera
#include "CameraMgr.h"

#include "EffectContainer.h"

CCharacterAttackCollider::CCharacterAttackCollider()
	: CGameObject()
{
}

CCharacterAttackCollider::CCharacterAttackCollider(const CCharacterAttackCollider& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCharacterAttackCollider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CRigidBody>();
	Add_Component<CCollider>();
	Add_Component<CBoneFollower>();

	return S_OK;
}

HRESULT CCharacterAttackCollider::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCharacterAttackCollider::Awake()
{
	Get_Component<CCollider>()->Set_CompActive(false);
}

void CCharacterAttackCollider::Priority_Update(_float dt)
{
	m_vPrevPos = m_pTransform->Get_Pos();
}

void CCharacterAttackCollider::Update(_float dt)
{
	m_fTimer += dt;

	Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);
	Get_Component<CRigidBody>()->Set_GlobalPos(m_pTransform->Get_Pos(), m_pTransform->Get_QuaternionRotate());
	Get_Component<CCollider>()->Update(dt);
}

void CCharacterAttackCollider::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CCharacterAttackCollider::Render_GUI()
{
	__super::Render_GUI();
}

void CCharacterAttackCollider::OnCollisionEnter(CGameObject* pOther)
{

}

void CCharacterAttackCollider::OnCollisionStay(CGameObject* pOther)
{

}

void CCharacterAttackCollider::OnCollisionExit(CGameObject* pOther)
{
}

void CCharacterAttackCollider::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::MONSTER))
		return;
	if (!Try_Hit(pOther))
		return;

	auto pEnemy = dynamic_cast<CEnemy*>(pOther);
	if (nullptr != pEnemy)
	{
		pEnemy->TakeDamage(m_tHitDesc.eDamageType, m_tHitDesc.fDamage);
		BattleSystem()->GetBattlePlayer()->Add_Gauge(m_tHitDesc.fEnergyCharge, m_tHitDesc.fDecibelCharge);

		/* Effect Test */
		_vector3 vWorldPosition = m_pTransform->Get_WorldPos();
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer"})
			.Asset("basic_hit.json")
			.Position(vWorldPosition)
			.Build("BasicHit");

		ObjectManager()->Add_Object(pEffect, { pEnemy->Get_Level(),"Effect_Layer" });

		// Camera
		auto pCharacter = dynamic_cast<CCharacter*>(Get_Component<CChild>()->Get_Parent());
		if(pCharacter != nullptr && pCharacter->Is_MainCharacter())
		{
			//CameraManager()->AddShake(CamShakeType::HitCrit);
			//CameraManager()->AddZoomPunch(0.8f, 0.045f, 0.15f);
			//CameraManager()->AddImpact(CamShakeType::TapSoft, CamZoomType::TapSoft, 1.5f);
			CameraManager()->AddImpact(1, 0);
		}
	}

}

void CCharacterAttackCollider::OnTriggerStay(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::MONSTER))
		return;
	if (!Try_Hit(pOther))
		return;
	
	auto pEnemy = dynamic_cast<CEnemy*>(pOther);
	if (nullptr != pEnemy)
	{
		pEnemy->TakeDamage(m_tHitDesc.eDamageType, m_tHitDesc.fDamage);
		BattleSystem()->GetBattlePlayer()->Add_Gauge(m_tHitDesc.fEnergyCharge, m_tHitDesc.fDecibelCharge);

		// Camera
		auto pCharacter = dynamic_cast<CCharacter*>(Get_Component<CChild>()->Get_Parent());
		if (pCharacter != nullptr && pCharacter->Is_MainCharacter())
		{
			CameraManager()->AddImpact(1, 0);
		}
	}
}

void CCharacterAttackCollider::OnTriggerExit(CGameObject* pOther)
{
}

void CCharacterAttackCollider::Begin_Attack(const HitDesc& hitdesc)
{
	m_tHitDesc = hitdesc;
	m_fTimer = 0.f;
	m_HitRecords.clear();
	Get_Component<CCollider>()->Set_CompActive(true);
}

void CCharacterAttackCollider::End_Attack()
{
	m_HitRecords.clear();
	Get_Component<CCollider>()->Set_CompActive(false);
}

_bool CCharacterAttackCollider::Try_Hit(CGameObject* pTarget)
{
	HitRecord& record = m_HitRecords[pTarget];

	switch (m_tHitDesc.eHitType)
	{
	case HIT_TYPE::ONCE:
		if (record.iHitCount >= 1)
			return false;
		break;

	case HIT_TYPE::INTERVAL:
		if (m_fTimer - record.fLastHitTime < m_tHitDesc.fInterval)
			return false;
		break;

	case HIT_TYPE::COUNT:
		if (record.iHitCount >= m_tHitDesc.iMaxCount)
			return false;
		if (record.iHitCount > 0 && m_fTimer - record.fLastHitTime < m_tHitDesc.fInterval)
			return false;
		break;
	}

	record.iHitCount++;
	record.fLastHitTime = m_fTimer;
	return true;
}

CCharacterAttackCollider* CCharacterAttackCollider::Create()
{
	CCharacterAttackCollider* instance = new CCharacterAttackCollider();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CCharacterAttackCollider");
	}

	return instance;
}

CGameObject* CCharacterAttackCollider::Clone(INIT_DESC* pArg)
{
	CCharacterAttackCollider* instance = new CCharacterAttackCollider(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CCharacterAttackCollider");
	}

	return instance;
}

void CCharacterAttackCollider::Free()
{
	__super::Free();
}
