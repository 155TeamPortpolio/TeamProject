#include "pch.h"
#include "EnemyTriggerCollider.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "Enemy.h"

/* Component */
#include "RigidBody.h"
#include "Collider.h"
#include "BoneFollower.h"
#include "Child.h"

CEnemyTriggerCollider::CEnemyTriggerCollider()
	: CGameObject()
{
}

CEnemyTriggerCollider::CEnemyTriggerCollider(const CEnemyTriggerCollider& rhg)
	: CGameObject(rhg)
{
}

HRESULT CEnemyTriggerCollider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CRigidBody>();
	Add_Component<CCollider>();
	Add_Component<CBoneFollower>();

	return S_OK;
}

HRESULT CEnemyTriggerCollider::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEnemyTriggerCollider::Awake()
{
	Get_Component<CCollider>()->Set_CompActive(false);
}

void CEnemyTriggerCollider::Priority_Update(_float dt)
{
}

void CEnemyTriggerCollider::Update(_float dt)
{
	Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);
	Get_Component<CRigidBody>()->Set_GlobalPos(m_pTransform->Get_Pos(), m_pTransform->Get_QuaternionRotate());
	Get_Component<CCollider>()->Update(dt);
}

void CEnemyTriggerCollider::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CEnemyTriggerCollider::Render_GUI()
{
	ImGui::PushID(this);
	
	if(ImGui::TreeNode("Inspector##EnemyTriggerColliderInspector"))
	{
		__super::Render_GUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Collider Check##EnemyTriggerColliderColliderCheck"))
	{
		if (ImGui::Button("Show Collider")) {
			auto pCollider = Get_Component<CCollider>();
			pCollider->Set_CompActive(!pCollider->Get_CompActive());
		}

		ImGui::TreePop();
	}
	ImGui::PopID();
}

void CEnemyTriggerCollider::OnTriggerEnter(CGameObject* pOther)
{
}

void CEnemyTriggerCollider::OnTriggerExit(CGameObject* pOther)
{
}

CEnemyTriggerCollider* CEnemyTriggerCollider::Create()
{
	CEnemyTriggerCollider* instance = new CEnemyTriggerCollider();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CEnemyTriggerCollider");
	}

	return instance;
}

CGameObject* CEnemyTriggerCollider::Clone(INIT_DESC* pArg)
{
	CEnemyTriggerCollider* instance = new CEnemyTriggerCollider(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CEnemyAttackCollider");
	}

	return instance;
}

void CEnemyTriggerCollider::Free()
{
	__super::Free();
}
