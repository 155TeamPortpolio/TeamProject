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
	//Add_Component<CCollider>();
	//Add_Component<CRigidBody>();
	Add_Component<CAudioSource>();

	return S_OK;
}

HRESULT CDefilerWall::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto desc = static_cast<DefilerWallDesc*>(pArg);
	m_pTransform->Set_Look(desc->vLook);
	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	for (const auto& instance : materialInstances)
	{
		instance->Override_Pass("Opaque");
		instance->Set_Param("fTime",{ &m_ElapsedTime,"float",  sizeof(_float) });
	}
	m_pTransform->Scale({ 0,0,0 });
	m_bAwake = true;

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
	if (!m_bAwake)
		return;

	m_ElapsedTime += dt;

	const _float duration = 0.25f;
	const _float t01 = clamp(m_ElapsedTime / duration, 0.f, 1.f);
	const _float eased = Math::ApplyEase(EaseType::OutExpo, t01);
	const _vector3 startScale = { 0.2f, 0.f, 0.2f };
	const _vector3 endScale = { 1.f, 1.f, 1.f };
	const _vector3 scale = startScale + (endScale - startScale) * eased;
	m_pTransform->Scale(scale); 

	if (t01 >= 1.f)
	{
		m_pTransform->Scale(endScale);  // 마감 고정
	}
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

void CDefilerWall::DisAppear()
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