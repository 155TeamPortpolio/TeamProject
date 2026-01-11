#include "pch.h"
#include "ThugBulkyEnforcer_Collider.h"

#include "Helper_Func.h"
#include "GameInstance.h"

/* Component */
#include "RigidBody.h"
#include "Collider.h"
#include "BoneFollower.h"

CThugBulkyEnforcer_Collider::CThugBulkyEnforcer_Collider()
	: CGameObject()
{
}

CThugBulkyEnforcer_Collider::CThugBulkyEnforcer_Collider(const CThugBulkyEnforcer_Collider& rhg)
	:CGameObject(rhg)
{
}

HRESULT CThugBulkyEnforcer_Collider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CRigidBody>();
	Add_Component<CCollider>();
	Add_Component<CBoneFollower>();

	return S_OK;
}

HRESULT CThugBulkyEnforcer_Collider::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CThugBulkyEnforcer_Collider::Awake()
{
}

void CThugBulkyEnforcer_Collider::Priority_Update(_float dt)
{
}

void CThugBulkyEnforcer_Collider::Update(_float dt)
{
	Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);

	Get_Component<CRigidBody>()->Set_GlobalPos(m_pTransform->Get_Pos(),m_pTransform->Get_QuaternionRotate());
	Get_Component<CCollider>()->Update(dt);
}

void CThugBulkyEnforcer_Collider::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CThugBulkyEnforcer_Collider::Render_GUI()
{
	ImGui::PushID(this);

	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

	if (ImGui::TreeNode("Inspector##ThugBulkyCollider")) {
		__super::Render_GUI();
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void CThugBulkyEnforcer_Collider::OnCollisionEnter(CGameObject* pOther)
{
	int a = 9;
}

void CThugBulkyEnforcer_Collider::OnCollisionStay(CGameObject* pOther)
{
	int a = 9;
}

void CThugBulkyEnforcer_Collider::OnCollisionExit(CGameObject* pOther)
{
	int a = 9;
}

CGameObject* CThugBulkyEnforcer_Collider::Clone(INIT_DESC* pArg)
{
	CThugBulkyEnforcer_Collider* instance = new CThugBulkyEnforcer_Collider(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CThugBulkyEnforcer_Collider");
	}

	return instance;
}

CThugBulkyEnforcer_Collider* CThugBulkyEnforcer_Collider::Create()
{
	CThugBulkyEnforcer_Collider* instance = new CThugBulkyEnforcer_Collider();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CThugBulkyEnforcer_Collider");
	}

	return instance;
}

void CThugBulkyEnforcer_Collider::Free()
{
	__super::Free();
}
