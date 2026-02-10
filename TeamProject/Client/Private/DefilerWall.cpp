#include "pch.h"
#include "DefilerWall.h"

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
#include "Texture.h"
#include "AudioSource.h"

CDefilerWall::CDefilerWall()
	: CEnemy()
{
}

CDefilerWall::CDefilerWall(const CDefilerWall& rhs)
	:CEnemy(rhs)
{
}

HRESULT CDefilerWall::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CStaticModel>()->Link_Model("Zero_Level", "Defiler_Wall.model");
	Add_Component<CMaterial>()->Link_Material("Zero_Level", "Defiler_Wall.mat");
	Add_Component<CCollider>();
	Add_Component<CRigidBody>();
	Add_Component<CAudioSource>();

	return S_OK;
}

HRESULT CDefilerWall::Initialize(INIT_DESC* pArg)
{
		return S_OK;
}

void CDefilerWall::Awake()
{

}

void CDefilerWall::Priority_Update(_float dt)
{
}
void CDefilerWall::Update(_float dt)
{
}
void CDefilerWall::Late_Update(_float dt)
{
}

void CDefilerWall::Render_GUI()
{
	__super::Render_GUI();
}

void CDefilerWall::OnPooledAcquire(INIT_DESC* pArg)
{
}

void CDefilerWall::OnPooledRelease()
{
	
}

CDefilerWall* CDefilerWall::Create()
{
	CDefilerWall* instance = new CDefilerWall();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefilerWall");
	}

	return instance;
}

CGameObject* CDefilerWall::Clone(INIT_DESC* pArg)
{
	CDefilerWall* instance = new CDefilerWall(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefilerWall");
	}

	return instance;
}

void CDefilerWall::Free()
{
	__super::Free();
}

void CDefilerWall::OnTriggerEnter(CGameObject* pOther)
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