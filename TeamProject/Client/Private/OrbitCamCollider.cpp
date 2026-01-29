#include "pch.h"
#include "OrbitCamCollider.h"
// Engine
#include "RigidBody.h"
#include "Collider.h"
#include "ICamCollidable.h"

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

void COrbitCamCollider::Late_Update(_float dt)
{
    Get_Component<CRigidBody>()->Late_Update(dt);

    for (auto& hPrev : m_prev)
    {
        _bool still = false;
        for (auto& hCur : m_cur)
        {
            if (hCur == hPrev) 
            {
                still = true;
                break; 
            }
        }

        if (still) continue;

        auto camCol = dynamic_cast<ICamCollidable*>(hPrev.Get());
        if (camCol) camCol->OnCameraCollision(false);
    }

    for (auto& hCur : m_cur)
    {
        _bool was = false;
        for (auto& hPrev : m_prev)
        {
            if (hPrev == hCur) 
            { 
                was = true;
                break;
            }
        }

        if (was) continue;

        auto camCol = dynamic_cast<ICamCollidable*>(hCur.Get());
        if (camCol) camCol->OnCameraCollision(true);
    }

    m_prev = m_cur;
    m_cur.clear();
}

void COrbitCamCollider::OnTriggerEnter(CGameObject* obj)
{
    auto camCol = dynamic_cast<ICamCollidable*>(obj);
    if (!camCol) return;

    OBJECT_HANDLE h = obj->Get_Handle();
    for (auto& e : m_cur)
    {
        if (e == h)
            return;
    }
    m_cur.push_back(h);
}

void COrbitCamCollider::OnTriggerStay(CGameObject* obj)
{
    auto camCol = dynamic_cast<ICamCollidable*>(obj);
    if (!camCol) return;

    OBJECT_HANDLE h = obj->Get_Handle();
    for (auto& e : m_cur)
    {
        if (e == h) 
            return;
    }
    m_cur.push_back(h);
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