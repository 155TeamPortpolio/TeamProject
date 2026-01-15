#include "pch.h"
#include "EnemyAttackCollider.h"

#include "Helper_Func.h"
#include "GameInstance.h"

#include "Enemy.h"

/* Component */
#include "RigidBody.h"
#include "Collider.h"
#include "BoneFollower.h"
#include "Child.h"

CEnemyAttackCollider::CEnemyAttackCollider()
	: CGameObject()
{
}

CEnemyAttackCollider::CEnemyAttackCollider(const CEnemyAttackCollider& rhg)
	: CGameObject(rhg)
{
}

HRESULT CEnemyAttackCollider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CRigidBody>();
	Add_Component<CCollider>();
	Add_Component<CBoneFollower>();

	return S_OK;
}

HRESULT CEnemyAttackCollider::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEnemyAttackCollider::Awake()
{
	Get_Component<CCollider>()->Set_CompActive(false);
}

void CEnemyAttackCollider::Priority_Update(_float dt)
{
}

void CEnemyAttackCollider::Update(_float dt)
{
	Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);
	Get_Component<CRigidBody>()->Set_GlobalPos(m_pTransform->Get_Pos(), m_pTransform->Get_QuaternionRotate());
	Get_Component<CCollider>()->Update(dt);
}

void CEnemyAttackCollider::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CEnemyAttackCollider::Render_GUI()
{
	ImGui::PushID(this);

	if (ImGui::TreeNode("Inspector##EnemyAttackColliderInspector"))
	{
		__super::Render_GUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Collider Check##EnemyAttackColliderColliderCheck"))
	{
		if (ImGui::Button("Show Collider")) {
			auto pCollider = Get_Component<CCollider>();
			pCollider->Set_CompActive(!pCollider->Get_CompActive());
		}

		ImGui::TreePop();
	}
	ImGui::PopID();
}

void CEnemyAttackCollider::OnCollisionEnter(CGameObject* pOther)
{
}

void CEnemyAttackCollider::OnCollisionStay(CGameObject* pOther)
{
}

void CEnemyAttackCollider::OnCollisionExit(CGameObject* pOther)
{
}

void CEnemyAttackCollider::OnTriggerEnter(CGameObject* pOther)
{
	static_cast<CEnemy*>(Get_Component<CChild>()->Get_Parent())->SetEnterAttackHit(true);
}

void CEnemyAttackCollider::OnTriggerExit(CGameObject* pOther)
{
	static_cast<CEnemy*>(Get_Component<CChild>()->Get_Parent())->SetEnterAttackHit(false);
}

CEnemyAttackCollider* CEnemyAttackCollider::Create()
{
	CEnemyAttackCollider* instance = new CEnemyAttackCollider();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CEnemyAttackCollider");
	}

	return instance;
}

CGameObject* CEnemyAttackCollider::Clone(INIT_DESC* pArg)
{
	CEnemyAttackCollider* instance = new CEnemyAttackCollider(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CEnemyAttackCollider");
	}

	return instance;
}

void CEnemyAttackCollider::Free()
{
	__super::Free();
}
