#include "pch.h"
#include "MiasmaProjectile.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

CMiasmaProjectile::CMiasmaProjectile()
	: CEnemy()
{
}

CMiasmaProjectile::CMiasmaProjectile(const CMiasmaProjectile& rhg)
	: CEnemy(rhg)
{
}

HRESULT CMiasmaProjectile::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>()->Link_Model("Zero_Level", "MiasmaProjectile.model");
	Add_Component<CMaterial>()->Link_Material("Zero_Level", "MiasmaProjectile.mat");
	Add_Component<CCollider>();
	Add_Component<CRigidBody>();

	return S_OK;
}

HRESULT CMiasmaProjectile::Initialize(INIT_DESC* pArg)
{
	auto desc = static_cast<MiasmaProjectileDesc*>(pArg);
	__super::Initialize(desc);
	m_isOnAttack = true;
	m_isParryEnable = false;

	auto pCollider = Get_Component<CCollider>();
	auto rigid = Get_Component<CRigidBody>();
	rigid->Set_Kinematic(false);
	rigid->Set_CCD(true);
	rigid->Set_Gravity(false);

	m_pTransform->LookAt(_vector3(desc->vTargetPos));
	m_vVelocity = { 0,0,0 };
	m_vTargetVelocity = m_pTransform->Dir(STATE::LOOK) * m_fMoveSpeed;
	m_ElapsedTime = 0;

	return S_OK;
}

void CMiasmaProjectile::Awake()
{
}

void CMiasmaProjectile::Priority_Update(_float dt)
{
}

void CMiasmaProjectile::Update(_float dt)
{
	m_ElapsedTime += dt;
	m_vVelocity = m_vVelocity.Lerp(m_vVelocity, m_vTargetVelocity, Math::ApplyEase(EaseType::InOutSine, m_ElapsedTime));
	Get_Component<CTransform>()->Translate(m_vVelocity*dt);
	Get_Component<CRigidBody>()->Set_GlobalPos(m_pTransform->Get_Pos(), m_pTransform->Get_QuaternionRotate());
	Get_Component<CCollider>()->Update(dt);
	if (m_ElapsedTime > 5.f)
		ObjectManager()->Remove_Object(this);
}

void CMiasmaProjectile::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CMiasmaProjectile::Render_GUI()
{
}

void CMiasmaProjectile::OnPooledAcquire(INIT_DESC* pArg)
{
	Initialize(pArg);
	Get_Component<CCollider>()->Set_CompActive(true);
}

void CMiasmaProjectile::OnPooledRelease()
{
	m_isOnAttack = false;
	m_vVelocity = { 0,0,0 };
	Get_Component<CCollider>()->Set_CompActive(false);
}

CMiasmaProjectile* CMiasmaProjectile::Create()
{
	CMiasmaProjectile* instance = new CMiasmaProjectile();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CMiasmaProjectile");
	}

	return instance;
}

CGameObject* CMiasmaProjectile::Clone(INIT_DESC* pArg)
{
	CMiasmaProjectile* instance = new CMiasmaProjectile(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMiasmaProjectile");
	}

	return instance;
}

void CMiasmaProjectile::Free()
{
	__super::Free();
}

void CMiasmaProjectile::OnTriggerEnter(CGameObject* pOther)
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
