#include "pch.h"
#include "ThugPoacher_Arrow.h"
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

CThugPoacher_Arrow::CThugPoacher_Arrow()
	: CEnemy()
{
}

CThugPoacher_Arrow::CThugPoacher_Arrow(const CThugPoacher_Arrow& rhg)
	: CEnemy(rhg)
{
}

HRESULT CThugPoacher_Arrow::Initialize_Prototype()
{
	Add_Component<CObjectContainer>();
	Add_Component<CCollider>();
	Add_Component<CRigidBody>();
	Add_Component<CMaterial>();
	Add_Component<CStaticModel>();

	return S_OK;
}

HRESULT CThugPoacher_Arrow::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	ARROW_DESC* pDesc = static_cast<ARROW_DESC*>(pArg);

	m_pWeaponBone = pDesc->pWeapon;

	Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "Default.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Default.mat");
	Get_Component<CTransform>()->Scale({ 0.2f, 0.2f, 0.2f });

	Get_Component<CRigidBody>()->Set_Kinematic(true);
	Get_Component<CCollider>()->Set_CompActive(false);
	m_isAlive = false;

	return S_OK;
}

void CThugPoacher_Arrow::Awake()
{
}

void CThugPoacher_Arrow::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CThugPoacher_Arrow::Update(_float dt)
{
	__super::Update(dt);

	m_pTransform->Translate(m_vDir * m_fSpeed * dt);
}

void CThugPoacher_Arrow::Late_Update(_float dt)
{
	__super::Late_Update(dt);

	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CThugPoacher_Arrow::Render_GUI()
{
	ImGui::PushID(this);
	__super::Render_GUI();
	ImGui::PopID();
}

void CThugPoacher_Arrow::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();

	if (nullptr == pCollidable)
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
		FinishArrow();


}

void CThugPoacher_Arrow::ShootArrow()
{
	_matrix ParentWorld = XMLoadFloat4x4(Get_Component<CChild>()->Get_Parent()->Get_Component<CTransform>()->Get_WorldMatrix_Ptr());
	_matrix WeaponBone = XMLoadFloat4x4(m_pWeaponBone);

	_matrix ResultMat = WeaponBone * ParentWorld;
	XMStoreFloat3(&m_vDir, XMVector3Normalize(ParentWorld.r[2]));

	_float3	vResultPos = {}; XMStoreFloat3(&vResultPos, ResultMat.r[3]);

	//Get_Component<CRigidBody>()->Set_GlobalPos(ResultMat.r[3], ParentWorld.r[2]);
	m_pTransform->Set_Pos(vResultPos);
	m_pTransform->Set_Look(XMVector3Normalize(ParentWorld.r[2]));

	Get_Component<CRigidBody>()->Late_Update(0);

	Get_Component<CCollider>()->Set_CompActive(true);
	m_isAlive = true;
}

void CThugPoacher_Arrow::FinishArrow()
{
	m_isAlive = false;
	Get_Component<CCollider>()->Set_CompActive(false);
	m_vDir = {};
}

CThugPoacher_Arrow* CThugPoacher_Arrow::Create()
{
	CThugPoacher_Arrow* instance = new CThugPoacher_Arrow();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CThugPoacher_Arrow");
	}

	return instance;
}

CGameObject* CThugPoacher_Arrow::Clone(INIT_DESC* pArg)
{
	CThugPoacher_Arrow* instance = new CThugPoacher_Arrow(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CThugPoacher_Arrow");
	}

	return instance;
}

void CThugPoacher_Arrow::Free()
{
	__super::Free();
}
