#include "pch.h"
#include "OrbitCamCollider.h"
// Engine
#include "RigidBody.h"
#include "Collider.h"

HRESULT COrbitCamCollider::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CRigidBody>();
	Add_Component<CCollider>();

	return S_OK;
}

HRESULT COrbitCamCollider::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);

	return S_OK;
}

void COrbitCamCollider::Priority_Update(_float dt)
{
}

void COrbitCamCollider::Update(_float dt)
{
}

void COrbitCamCollider::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void COrbitCamCollider::OnTriggerEnter(CGameObject* obj)
{
}

void COrbitCamCollider::OnTriggerStay(CGameObject* obj)
{
}

void COrbitCamCollider::OnTriggerExit(CGameObject* obj)
{
}

COrbitCamCollider* COrbitCamCollider::Create()
{
	auto inst = new COrbitCamCollider();
	if (FAILED(inst->Initialize_Prototype()))
	{
		Safe_Release(inst);
		MSG_BOX("Failed to create : COrbitCamCollider");
	}
	return inst;
}

CGameObject* COrbitCamCollider::Clone(INIT_DESC* pArg)
{
	auto inst = new COrbitCamCollider(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		Safe_Release(inst);
		MSG_BOX("Failed to clone : COrbitCamCollider");
	}
	return inst;
}