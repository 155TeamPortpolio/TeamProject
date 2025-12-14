#include "Engine_Defines.h"
#include "CollisionSystem.h"
#include "GameInstance.h"
#include "ICameraService.h"
#include "Collider.h"

void CCollisionSystem::CPhysXEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	if (m_pOwner) m_pOwner->Process_Contact(pairHeader, pairs, nbPairs);
}

void CCollisionSystem::CPhysXEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	if (m_pOwner) m_pOwner->Process_Trigger(pairs, count);
}

CCollisionSystem::CCollisionSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice{ pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}

HRESULT CCollisionSystem::Initialize()
{

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t      iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	m_Colliders.reserve(1000);
#endif


	// 프록시 콜백
	m_pPhysXCallback = new CPhysXEventCallback(this);
	// 씬에 등록
	PxScene* pScene = CGameInstance::GetInstance()->Get_PhysicsSystem()->Get_Scene();
	if (pScene)
	{
		pScene->setSimulationEventCallback(m_pPhysXCallback);
	}

	return S_OK;
}

void CCollisionSystem::Update(_float dt)
{
#ifdef _DEBUG
	// 디버그용 리스트 관리
	for (auto it = m_Colliders.begin(); it != m_Colliders.end();)
	{
		if ((*it) == nullptr) // 이미 해제된 경우
			it = m_Colliders.erase(it);
		else
			++it;
	}
#endif
}

void CCollisionSystem::Process_Contact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		// 쉐이프가 삭제되었으면 스킵
		if (cp.flags & (PxContactPairFlag::eREMOVED_SHAPE_0 | PxContactPairFlag::eREMOVED_SHAPE_1))
			continue;

		// UserData에서 Collider 추출
		if (cp.shapes[0]->userData && cp.shapes[1]->userData)
		{
			auto pColA = static_cast<CCollider*>(cp.shapes[0]->userData);
			auto pColB = static_cast<CCollider*>(cp.shapes[1]->userData);

			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				pColA->OnCollisionEnter(pColB, cp);
				pColB->OnCollisionEnter(pColA, cp);
			}
			else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				pColA->OnCollisionExit(pColB);
				pColB->OnCollisionExit(pColA);
			}
		}
	}
}

void CCollisionSystem::Process_Trigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		auto pTrigger = static_cast<CCollider*>(pairs[i].triggerShape->userData);
		auto pOther = static_cast<CCollider*>(pairs[i].otherShape->userData);

		if (pTrigger && pOther)
		{
			if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				pTrigger->OnTriggerEnter(pOther);
				pOther->OnTriggerEnter(pTrigger);
			}
			else if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				pTrigger->OnTriggerExit(pOther);
				pOther->OnTriggerExit(pTrigger);
			}
		}
	}
}

#ifdef _DEBUG
_int CCollisionSystem::RegisterCollider(CCollider* pCollider, _int Index)
{
	m_Colliders.push_back(pCollider);
	return m_Colliders.size() - 1;
}

void CCollisionSystem::UnregisterCollider(CCollider* pCollider, _int Index)
{
	for (size_t i = 0; i < m_Colliders.size(); ++i)
	{
		if (m_Colliders[i] == pCollider)
		{
			// 제거할 대상을 마지막 요소와 교체하고 pop_back
			if (i != m_Colliders.size() - 1)
			{
				swap(m_Colliders[i], m_Colliders.back());
			}
			m_Colliders.pop_back();
			return;
		}
	}
}

void CCollisionSystem::Render_Debug()
{
	if (m_Colliders.empty()) return;

	auto camMgr = CGameInstance::GetInstance()->Get_CameraMgr();
	_matrix viewMat = XMLoadFloat4x4(camMgr->Get_ViewMatrix());
	_matrix projMat = XMLoadFloat4x4(camMgr->Get_ProjMatrix());

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(viewMat);
	m_pEffect->SetProjection(projMat);

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	// 등록된 콜라이더들만 그림
	for (auto& pCollider : m_Colliders)
	{
		if (pCollider && pCollider->Get_CompActive())
		{
			XMVECTOR vColor = pCollider->IsColliding() ? Colors::Red : Colors::Green;
			pCollider->Render(m_pBatch, vColor);
		}
	}

	m_pBatch->End();
}
#endif 

CCollisionSystem* CCollisionSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollisionSystem* Instance = new CCollisionSystem(pDevice, pContext);
	if (FAILED(Instance->Initialize())) {
		Safe_Release(Instance);
	}
	return Instance;
}

void CCollisionSystem::Free()
{
	if (m_pPhysXCallback)
	{
		delete m_pPhysXCallback;
		m_pPhysXCallback = nullptr;
	}


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	m_Colliders.clear();
	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pEffect);
	Safe_Delete(m_pBatch);
#endif 

	__super::Free();
}

#pragma region OLD
//void CCollisionSystem::DeActiveCollider(CCollider* pCollider, _int Index)
//{
//	if (Index >= m_Colliders.size()) {
//		return;
//	}
//
//	if (m_Colliders[Index].pCollider != pCollider) {
//		return;
//	}
//
//	else {
//		m_Colliders[Index].eState = COLLIDER_SLOT::STATE::INACTIVE;
//	}
//}
//
//void CCollisionSystem::ActiveCollider(CCollider* pCollider, _int Index)
//{
//	if (Index >= m_Colliders.size()) {
//		return;
//	}
//	if (m_Colliders[Index].pCollider != pCollider) {
//		return;
//	}
//	if (!pCollider->Has_Desc()) {
//		return;
//	}
//
//	else {
//		m_Colliders[Index].eState = COLLIDER_SLOT::STATE::ACTIVE;
//	}
//}

//void CCollisionSystem::Clean_Up()
//{
//	for (auto& col : m_Colliders) {
//		if (!col.pCollider) continue;
//
//		if (false == col.pCollider->Get_CompActive()) {
//			col.pCollider = nullptr;
//			col.eState = COLLIDER_SLOT::STATE::DEAD;
//		}
//	}
//}

//void CCollisionSystem::MakeCandidate()
//{
//	m_CandidateCollision.clear();
//	m_CandidateCollision.reserve(m_Colliders.size());
//
//	for (size_t i = 0; i < m_Colliders.size(); i++)
//	{
//		if (m_Colliders[i].IsValid() == false)
//			continue;
//		if (m_Colliders[i].IsActive() == false)
//			continue;
//		if (m_Colliders[i].pCollider->Get_CompActive() == false)
//			continue;
//
//		for (size_t j = i + 1; j < m_Colliders.size(); j++)
//		{
//			if (m_Colliders[j].IsValid() == false)
//				continue;
//			if (m_Colliders[j].IsActive() == false)
//				continue;
//			if (m_Colliders[j].pCollider->Get_CompActive() == false)
//				continue;
//
//			m_CandidateCollision.emplace_back(i, j);
//		}
//	}
//}
#pragma endregion