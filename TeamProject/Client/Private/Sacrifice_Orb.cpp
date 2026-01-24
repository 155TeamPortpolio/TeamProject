#include "pch.h"
#include "Sacrifice_Orb.h"
#include "BattleSystem.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "ObjectMgr.h"

/* Object */
#include "EffectContainer.h"

/* Component */
#include "ObjectContainer.h"
#include "RigidBody.h"
#include "Collider.h"

CSacrifice_Orb::CSacrifice_Orb()
	:CGameObject()
{
}

CSacrifice_Orb::CSacrifice_Orb(const CSacrifice_Orb& rhg)
	:CGameObject(rhg)
{
}

HRESULT CSacrifice_Orb::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	Add_Component<CRigidBody>();
	Add_Component<CCollider>();
	return S_OK;
}

HRESULT CSacrifice_Orb::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	COLLIDER_DESC colliderDesc = {};
	colliderDesc.eGroup = COLLISION_GROUP::MONSTER_ATTACK;
	colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
	colliderDesc.bTrigger = true;
	colliderDesc.bAutoFit = false;
	colliderDesc.eType = COLLIDER_TYPE::SPHERE;
	colliderDesc.vSize = _float3{ 2.f,2.f,2.f };
	colliderDesc.fSizeScale = 1.f;
	colliderDesc.vCenter = _float3{ 0.f,0.f,0.f };
	colliderDesc.vRotation = _float3{ 0.f,0.f,0.f };

	auto pCollider = Get_Component<CCollider>()->Initialize(&colliderDesc);

	auto pRigidBody = Get_Component<CRigidBody>();
	pRigidBody->Set_Kinematic(true);
	 
	auto pOrb = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("sacrifice_orb.json")
		.Build("Sacrifice_Orb");

	auto pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Add_Child(pOrb);

	m_fSpeed = 70.f;
	return S_OK;
}

void CSacrifice_Orb::Awake()
{
}

void CSacrifice_Orb::Priority_Update(_float dt)
{
}

void CSacrifice_Orb::Update(_float dt)
{
	/* Chase Target */
	{
		auto pRigidBody = Get_Component<CRigidBody>();
		auto battle = CBattleSystem::GetInstance();

		_vector3 vCurrPosition = m_pTransform->Get_WorldPos();
		_vector3 vCurrDir = m_pTransform->Dir(STATE::LOOK);
		_vector3 vTargetDir{};

		auto playerHandle = BattleSystem()->GetCurCharacterHandle();
		_vector3 targetPos = playerHandle.Get()->Get_Component<CTransform>()->Get_WorldPos();
		vTargetPos = targetPos;

		vTargetDir = vTargetPos - vCurrPosition;
		vTargetDir.y = 0.f;
		vTargetDir.Normalize();

		vTargetDir = _vector3::Lerp(vCurrDir, vTargetDir, dt * 30.f);
		m_pTransform->Set_Look(vTargetDir);
		m_pTransform->Translate(vTargetDir * m_fSpeed * dt);
	}

	auto pObjectContainer = Get_Component<CObjectContainer>();
	auto pCollider = Get_Component<CCollider>();
	
	pCollider->Update(dt);
	pObjectContainer->UpdateChild(dt);
}

void CSacrifice_Orb::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CSacrifice_Orb::Render_GUI()
{
	__super::Render_GUI();
	_vector3 velo = Get_Component<CRigidBody>()->Get_Velocity();

	ImGui::Text("pos %f,%f,%f", velo.x, velo.y, velo.z);
}

void CSacrifice_Orb::OnTriggerEnter(CGameObject* pOther)
{
	ObjectManager()->Remove_Object(this); 

	_vector3 vPosition = m_pTransform->Get_WorldPos();
	auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("sacrifice_orb_explode.json")
		.Position(vPosition)
		.Build("Sacrifice_Orb_Explode");

	ObjectManager()->Add_Object(effect, { Get_Level(),"Enemy_Effect_Layer" });
}

CSacrifice_Orb* CSacrifice_Orb::Create()
{
	CSacrifice_Orb* instance = new CSacrifice_Orb();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSacrifice_Orb");
	}

	return instance;
}
CGameObject* CSacrifice_Orb::Clone(INIT_DESC* pArg)
{
	CSacrifice_Orb* instance = new CSacrifice_Orb(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrifice_Orb");
	}

	return instance;
}

void CSacrifice_Orb::Free()
{
	__super::Free();
}

void CSacrifice_Orb::ChaseTarget()
{

}
