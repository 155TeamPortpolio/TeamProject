#include "AABB_Collider.h"
#include "OBB_Collider.h"
#include "Sphere_Collider.h"
#include "Transform.h"
#include "GameObject.h"
#include "Build_Struct.h"
#include "GameInstance.h"
#include "ICollisionService.h"
#ifdef _DEBUG
#include "DebugDraw.h"
#endif

CAABB_Collider::CAABB_Collider()
{
}

CAABB_Collider::CAABB_Collider(const CAABB_Collider& rhs)
	:CCollider(rhs)
{
}

HRESULT CAABB_Collider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAABB_Collider::Initialize(COMPONENT_DESC* pArg)
{
	__super::Initialize(pArg);
	if (pArg != nullptr) {
		AABB_COLLIDER_DESC* pDesc = static_cast<AABB_COLLIDER_DESC*>(pArg);
		m_pOriginalDesc = new BoundingBox(pDesc->vCenter, pDesc->vSize);
		m_pDesc = new BoundingBox(*m_pOriginalDesc);
		m_SystemIndex = CGameInstance::GetInstance()->Get_CollisionSystem()->RegisterCollider(this, m_SystemIndex);
	}
	return S_OK;
}

void CAABB_Collider::Update()
{
	if ((m_pDesc == nullptr) || (m_pOriginalDesc == nullptr)) return;


	_float4x4 OwnerMatrix = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix();
	_matrix     TransformMatrix = XMLoadFloat4x4(&OwnerMatrix);

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);

	m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

void CAABB_Collider::Late_Update()
{
	if ((m_pDesc == nullptr) || (m_pOriginalDesc == nullptr))
		return;

	for (auto& currSlot : m_CurrentCollider)
	{
		if (currSlot->IsValid() == false)
			continue;

		/*유효하지 않은 현재 충돌 대상 필터링*/
		CCollider* pCol = currSlot->pCollider;
		if (pCol->Get_CompActive() == false)
			continue;

		// 이전 프레임 동일 슬롯
		auto itPrev = find_if(m_prevCollider.begin(), m_prevCollider.end(),
			[&](COLLIDER_SLOT* prevSlot)
			{
				return prevSlot != nullptr && Compare_Same(prevSlot, currSlot);
			});

		if (itPrev == m_prevCollider.end())
		{//찾아보니 없음
			m_pOwner->OnCollisionEnter(pCol->Get_Context());
		}
		else
		{//찾아보니 있음
			m_pOwner->OnCollisionStay(pCol->Get_Context());
		}
	}

	for (auto& prevSlot : m_prevCollider)
	{
		/*유효하지 않은 이전 충돌 대상 필터링*/
		if (prevSlot->IsValid() == false)
			continue;

		/*유효하지 않은 현재 충돌 대상 필터링*/
		CCollider* pCol = prevSlot->pCollider;
		if (pCol->Get_CompActive() == false) {
			m_pOwner->OnCollisionExit(pCol->Get_Context());
			continue;
		}

		auto itCurr = find_if(m_CurrentCollider.begin(), m_CurrentCollider.end(),
			[&](COLLIDER_SLOT* currSlot)
			{
				return currSlot != nullptr && Compare_Same(prevSlot, currSlot);
			});

		if (itCurr == m_CurrentCollider.end())
		{
			m_pOwner->OnCollisionExit(pCol->Get_Context());
		}
	}

	m_prevCollider.swap(m_CurrentCollider);
	m_CurrentCollider.clear();

}

_bool CAABB_Collider::Intersect(COLLIDER_SLOT* pSlot)
{
	_bool       onCollision = { false };

	if (pSlot->IsValid() == false || pSlot->IsActive()==false)
		return false;

	if (pSlot->pCollider->Get_CompActive() == false)
		return false;

	switch (pSlot->pCollider->Get_ColliderType())        
	{
	case Engine::COLLIDER_TYPE::AABB:
		onCollision = m_pDesc->Intersects(*(static_cast<CAABB_Collider*>(pSlot->pCollider)->Get_Desc()));
		break;
	case Engine::COLLIDER_TYPE::OBB:
		onCollision = m_pDesc->Intersects(*(static_cast<COBB_Collider*>(pSlot->pCollider)->Get_Desc()));
		break;
	case Engine::COLLIDER_TYPE::SPHERE:
		onCollision = m_pDesc->Intersects(*(static_cast<CSphere_Collider*>(pSlot->pCollider)->Get_Desc()));
		break;
	default:
		break;
	}

	if (onCollision) {
		m_CurrentCollider.insert(pSlot);
	}

	return onCollision;
}

void CAABB_Collider::Make_MinMaxCollider(MINMAX_BOX minMax)
{
	if (m_pOriginalDesc) {
		Safe_Delete(m_pOriginalDesc);
		Safe_Delete(m_pDesc);
	}

	_float3 vCenter = {
	(minMax.vMax.x + minMax.vMin.x)*0.5f,
	(minMax.vMax.y + minMax.vMin.y) * 0.5f,
	(minMax.vMax.z + minMax.vMin.z) * 0.5f
	};
	_float3 vSize = {
	(minMax.vMax.x - minMax.vMin.x) * 0.5f,
	(minMax.vMax.y - minMax.vMin.y) * 0.5f,
	(minMax.vMax.z - minMax.vMin.z) * 0.5f
	};

	m_pOriginalDesc = new BoundingBox(vCenter,vSize);
	m_pDesc = new BoundingBox(*m_pOriginalDesc);
	m_SystemIndex = CGameInstance::GetInstance()->Get_CollisionSystem()->RegisterCollider(this, m_SystemIndex);
}

#ifdef _DEBUG
void CAABB_Collider::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);
	return;
}
#endif

CAABB_Collider* CAABB_Collider::Create()
{
	CAABB_Collider* instance = new CAABB_Collider();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("CAABB_Collider Comp Failed To Create : CAABB_Collider");
	}
	return instance;
}

CComponent* CAABB_Collider::Clone()
{
	return new CAABB_Collider(*this);
}

void CAABB_Collider::Free()
{
	__super::Free();
	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}
