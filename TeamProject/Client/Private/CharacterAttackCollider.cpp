#include "pch.h"
#include "CharacterAttackCollider.h"

#include "RigidBody.h"
#include "Collider.h"
#include "BoneFollower.h"
#include "Child.h"

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
}

void CCharacterAttackCollider::Priority_Update(_float dt)
{
}

void CCharacterAttackCollider::Update(_float dt)
{
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
	//MSG_BOX("Ãæµ¹¤»¤»!");
}

void CCharacterAttackCollider::OnCollisionStay(CGameObject* pOther)
{
}

void CCharacterAttackCollider::OnCollisionExit(CGameObject* pOther)
{
}

void CCharacterAttackCollider::OnTriggerEnter(CGameObject* pOther)
{
}

void CCharacterAttackCollider::OnTriggerStay(CGameObject* pOther)
{
}

void CCharacterAttackCollider::OnTriggerExit(CGameObject* pOther)
{
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
