#include "pch.h"
#include "Sacrifice_Orb.h"
#include "BattleSystem.h"
#include "Helper_Func.h"

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

		auto& battleInfos = battle->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
		_vector3 vCurrPosition = m_pTransform->Get_WorldPos();

		_vector3 vCurrDir = m_pTransform->Dir(STATE::LOOK);
		_vector3 vTargetDir{};

		for (auto& info : battleInfos)
		{
			if (info.isOnField) 
			{
				vTargetPos = info.vPos;
				break;
			}
		}

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

void CSacrifice_Orb::OnCollisionEnter(CGameObject* pOther)
{

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
