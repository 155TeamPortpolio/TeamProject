#include "pch.h"
#include "MeleeJaeger_Shield.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Component */
#include "Material.h"
#include "StaticModel.h"
#include "ObjectContainer.h"
#include "Collider.h"
#include "Child.h"
#include "BoneFollower.h"

CMeleeJaeger_Shield::CMeleeJaeger_Shield()
	: CEnemy()
{
}

CMeleeJaeger_Shield::CMeleeJaeger_Shield(const CMeleeJaeger_Shield& rhg)
	: CEnemy(rhg)
{
}

HRESULT CMeleeJaeger_Shield::Initialize_Prototype()
{
	Add_Component<CObjectContainer>();
	Add_Component<CCollider>();
	Add_Component<CRigidBody>();
	Add_Component<CMaterial>();
	Add_Component<CStaticModel>();
	Add_Component<CBoneFollower>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("MeleeJaeger_Shield.mat", "../Bin/Resources/Zero/Enemy/MeleeJaeger_Shield/MeleeJaeger_Shield.mat");
	pResourceMgr->Add_ResourcePath("MeleeJaeger_Shield.model", "../Bin/Resources/Zero/Enemy/MeleeJaeger_Shield/MeleeJaeger_Shield.model");

	auto pModel = Get_Component<CStaticModel>();
	pModel->Link_Model(G_GlobalLevelKey, "MeleeJaeger_Shield.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "MeleeJaeger_Shield.mat");

	return S_OK;
}

HRESULT CMeleeJaeger_Shield::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	JAEGERSHIELD_DESC* pDesc = static_cast<JAEGERSHIELD_DESC*>(pArg);

	m_pHandBone = pDesc->pHandBone;
	if (nullptr == pDesc->pHandBone)
		return E_FAIL;

	Get_Component<CRigidBody>()->Set_Kinematic(true);

	m_vOffset = { -0.05f, -0.03f, 0.17f };

	return S_OK;
}

void CMeleeJaeger_Shield::Awake()
{
	__super::Awake();
}

void CMeleeJaeger_Shield::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CMeleeJaeger_Shield::Update(_float dt)
{
	ComputePosition();

	__super::Update(dt);
}

void CMeleeJaeger_Shield::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
	__super::Late_Update(dt);
}

void CMeleeJaeger_Shield::Render_GUI()
{
	ImGui::PushID(this);
	__super::Render_GUI();

	float v[3] = { m_vOffset.x, m_vOffset.y, m_vOffset.z };

	if (ImGui::DragFloat3("##ShieldOffset", v, 0.01f))  // speed = 0.1
	{
		m_vOffset.x = v[0];
		m_vOffset.y = v[1];
		m_vOffset.z = v[2];
	}

	ImGui::PopID();
}

void CMeleeJaeger_Shield::OnTriggerEnter(CGameObject* pOther)
{
}

void CMeleeJaeger_Shield::ComputePosition(_bool isFirst)
{
	auto pChildCom = Get_Component<CChild>();
	if (nullptr == pChildCom)
		return;

	auto pParent = pChildCom->Get_Parent();
	_matrix mLocal = XMLoadFloat4x4(m_pTransform->Get_WorldMatrix_Ptr());
	_matrix mHandBone = XMLoadFloat4x4(m_pHandBone);
	_matrix mParentWorld = XMLoadFloat4x4(pParent->Get_WorldMatrix_Ptr());

	_matrix mResult = mLocal * mHandBone * mParentWorld;

	_vector vPos = mResult.r[3];


	if (m_isFirstCompute)
	{
		_vector vRight = XMVector3Normalize(mParentWorld.r[0]);
		_vector vUp = XMVector3Normalize(mParentWorld.r[1]);
		_vector vLook = XMVector3Normalize(mParentWorld.r[2]);
		
		_vector vOffsetWorld =
			vRight * m_vOffset.x +
			vUp * m_vOffset.y +
			vLook * m_vOffset.z;

		vPos += vOffsetWorld;
	}
	_float3 vResultPos = {}; XMStoreFloat3(&vResultPos, vPos);

	m_pTransform->Set_Pos(vResultPos);
	m_pTransform->Set_Quaternion(pParent->Get_Component<CTransform>()->Get_QuaternionRotate());

	if (!m_isFirstCompute)
		m_isFirstCompute = true;
}

CMeleeJaeger_Shield* CMeleeJaeger_Shield::Create()
{
	CMeleeJaeger_Shield* instance = new CMeleeJaeger_Shield();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CCyclops_Spit");
	}

	return instance;
}

CGameObject* CMeleeJaeger_Shield::Clone(INIT_DESC* pArg)
{
	CMeleeJaeger_Shield* instance = new CMeleeJaeger_Shield(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMeleeJaeger_Shield");
	}

	return instance;
}

void CMeleeJaeger_Shield::Free()
{
	__super::Free();
}
