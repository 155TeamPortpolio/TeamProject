#include "pch.h"
#include "Cyclops_Spit.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Component */
#include "ObjectContainer.h"
#include "Collider.h"
#include "Child.h"

//임시
#include "Material.h"
#include "StaticModel.h"

#include "Character.h"

CCyclops_Spit::CCyclops_Spit()
	: CEnemy()
{
}

CCyclops_Spit::CCyclops_Spit(const CCyclops_Spit& rhg)
	: CEnemy(rhg)
{
}

HRESULT CCyclops_Spit::Initialize_Prototype()
{
	Add_Component<CObjectContainer>();
	Add_Component<CCollider>();
	Add_Component<CRigidBody>();
	Add_Component<CMaterial>();
	Add_Component<CStaticModel>();

	return S_OK;
}

HRESULT CCyclops_Spit::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	SPIT_DESC* pDesc = static_cast<SPIT_DESC*>(pArg);

	m_pHeadBone = pDesc->pHeadBone;

	Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "Default.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Default.mat");
	Get_Component<CTransform>()->Scale({ 0.2f, 0.2f, 0.2f });

	Get_Component<CRigidBody>()->Set_Kinematic(true);
	Get_Component<CCollider>()->Set_CompActive(false);
	m_isAlive = false;

	return S_OK;
}

void CCyclops_Spit::Awake()
{
}

void CCyclops_Spit::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CCyclops_Spit::Update(_float dt)
{
	__super::Update(dt);
	
	// 처음 쏠 때, 충돌 무적
	if (m_isCollisionCooltime)
	{
		m_vCollisionCooltime.y += dt;
		if (m_vCollisionCooltime.x <= m_vCollisionCooltime.y)
		{
			m_isCollisionCooltime = false;
			m_vCollisionCooltime.y = 0.f;
		}
	}

	if (m_isStraight)
		m_pTransform->Translate(m_vDir * m_fShootSpeed * dt);

	if (m_isArc)
	{
		m_fLifeTime += dt;

		_vector p = m_pTransform->Get_Pos();
		_vector v = XMLoadFloat3(&m_vVelocity);

		// 중력 스케일 램프업 (초반엔 거의 직선, 시간이 지나며 포물선이 강해짐)
		_float gravityScale = ComputeGravityScale(m_fLifeTime, m_fRampTime);

		_vector a = XMVectorSet(0.f, -m_fGravity * gravityScale, 0.f, 0.f);

		// Semi-implicit Euler: v 먼저 갱신하고 p 이동
		v += a * dt;
		p += v * dt;

		_float3 vResultPos = {}; XMStoreFloat3(&vResultPos, p);

		XMStoreFloat3(&m_vVelocity, v);
		m_pTransform->Set_Pos(vResultPos);
	}

}

void CCyclops_Spit::Late_Update(_float dt)
{
	__super::Late_Update(dt);

	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CCyclops_Spit::Render_GUI()
{
	ImGui::PushID(this);
	__super::Render_GUI();
	ImGui::PopID();
}

void CCyclops_Spit::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();

	if (nullptr == pCollidable || true == m_isCollisionCooltime)
		return;

	_bool isCollision = false;
	COLLISION_GROUP eGroup = pCollidable->Get_Group();

	switch (eGroup)
	{
	case Engine::COLLISION_GROUP::COMMON:
	{
		isCollision = true;
		break;
	}
	case Engine::COLLISION_GROUP::PLAYER:
	{
		// 데미지 주는 코드
		auto pEnemy = dynamic_cast<CCharacter*>(pOther);
		if (nullptr != pEnemy)
		{
			pEnemy->Take_Damage(DAMAGE_TYPE::NORMAL, 10);
			CameraManager()->AddImpact(1, 0);
			isCollision = true;
		}
		break;
	}
	case Engine::COLLISION_GROUP::MONSTER:
		break;
	case Engine::COLLISION_GROUP::PLAYER_ATTACK:
		break;
	case Engine::COLLISION_GROUP::MONSTER_ATTACK:
		break;
	case Engine::COLLISION_GROUP::MONSTER_PARRY:
		break;
	case Engine::COLLISION_GROUP::CAMERA:
		break;
	case Engine::COLLISION_GROUP::INTERACTABLE:
	{
		isCollision = true;
		break;
	}
	}

	if (true == isCollision)
		FinishSpit();
}

void CCyclops_Spit::ShootSpit(SPIT eSpitType)
{
	_matrix ParentWorld = XMLoadFloat4x4(Get_Component<CChild>()->Get_Parent()->Get_Component<CTransform>()->Get_WorldMatrix_Ptr());
	_matrix HeadBone = XMLoadFloat4x4(m_pHeadBone);

	_matrix ResultMat = HeadBone * ParentWorld;

	_float3	vResultPos = {}; XMStoreFloat3(&vResultPos, ResultMat.r[3]);

	// 침 위치 세팅
	m_pTransform->Set_Pos(vResultPos);
	Get_Component<CRigidBody>()->Late_Update(0);
	
	_vector vLookDir = XMVector3Normalize(ParentWorld.r[2]);

	switch (eSpitType)
	{
	case Client::CCyclops_Spit::SPIT::STRAIGHT:
	{
		m_pTransform->Set_Look(vLookDir);
		XMStoreFloat3(&m_vDir, vLookDir);

		m_isStraight = true;
		break;
	}
	case Client::CCyclops_Spit::SPIT::ARC_CENTER:
	{
		m_pTransform->Set_Look(vLookDir);
		XMStoreFloat3(&m_vDir, vLookDir);

		XMVECTOR v0 = vLookDir * m_fArcSpeed + XMVectorSet(0.f, m_fUpKick, 0.f, 0.f);
		XMStoreFloat3(&m_vVelocity, v0);

		m_isArc = true;
		break;
	}
	case Client::CCyclops_Spit::SPIT::ARC_LEFT:
	{
		_vector vDir = MakeSpreadDir_Yaw(vLookDir, -15.f);

		m_pTransform->Set_Look(vDir);
		XMStoreFloat3(&m_vDir, vDir);

		XMVECTOR v0 = vDir * m_fArcSpeed + XMVectorSet(0.f, m_fUpKick, 0.f, 0.f);
		XMStoreFloat3(&m_vVelocity, v0);

		m_isArc = true;
		break;
	}
	case Client::CCyclops_Spit::SPIT::ARC_RIGHT:
	{
		_vector vDir = MakeSpreadDir_Yaw(vLookDir, 15.f);

		m_pTransform->Set_Look(vDir);
		XMStoreFloat3(&m_vDir, vDir);

		XMVECTOR v0 = vDir * m_fArcSpeed + XMVectorSet(0.f, m_fUpKick, 0.f, 0.f);
		XMStoreFloat3(&m_vVelocity, v0);

		m_isArc = true; 
		break;
	}
	}

	Get_Component<CCollider>()->Set_CompActive(true);
	Get_Component<CRigidBody>()->Late_Update(0);

	m_isCollisionCooltime = true;
	m_isAlive = true;
}

void CCyclops_Spit::FinishSpit()
{
	m_isStraight = false;
	m_isArc = false;

	m_isAlive = false;
	Get_Component<CCollider>()->Set_CompActive(false);
	m_vDir = {};
}

_float CCyclops_Spit::ComputeGravityScale(_float lifeTime, _float rampTime)
{
	// rampTime <= 0이면 즉시 중력 100%
	if (rampTime <= 0.f)
		return 1.f;

	_float t = lifeTime / rampTime;

	return Math::SmoothStep01(t);
}

_vector CCyclops_Spit::MakeSpreadDir_Yaw(_fvector vLookDir, _float fYawDeg)
{
	_vector d = vLookDir;

	// XZ 기준으로만 퍼지게 (y 제거)
	d = XMVectorSet(XMVectorGetX(d), 0.f, XMVectorGetZ(d), 0.f);

	// 혹시 길이 0이면 안전 처리
	if (XMVector3Less(XMVector3LengthSq(d), XMVectorReplicate(1e-6f)))
		d = XMVectorSet(0.f, 0.f, 1.f, 0.f); // 기본 전방

	d = XMVector3Normalize(d);

	const _float rad = XMConvertToRadians(fYawDeg);
	_vector up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	// up축 기준 회전 (좌: -, 우: +)
	_matrix rot = XMMatrixRotationAxis(up, rad);
	_vector outDir = XMVector3TransformNormal(d, rot);

	return XMVector3Normalize(outDir);
}

CCyclops_Spit* CCyclops_Spit::Create()
{
	CCyclops_Spit* instance = new CCyclops_Spit();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CCyclops_Spit");
	}

	return instance;
}

CGameObject* CCyclops_Spit::Clone(INIT_DESC* pArg)
{
	CCyclops_Spit* instance = new CCyclops_Spit(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CCyclops_Spit");
	}

	return instance;
}

void CCyclops_Spit::Free()
{
	__super::Free();
}
