#include "Engine_Defines.h"
#include "CollisionSystem.h"
#include "GameInstance.h"
#include "ICameraService.h"
#include "Collider.h"
#include "CharacterController.h"

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
#endif

	m_Collidables.reserve(1000);

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
	for (auto it = m_Collidables.begin(); it != m_Collidables.end();)
	{
		if ((*it) == nullptr) // 이미 해제된 경우
			it = m_Collidables.erase(it);
		else
			++it;
	}
}

void CCollisionSystem::Render_GUI()
{
	/*ImGui::Begin("Collision System");

	ImGui::Text("Total Colliders: %d", m_Collidables.size());

	 활성/비활성 카운트
	_uint iActiveCount = 0;
	_uint iTriggerCount = 0;
	_uint iCollidingCount = 0;

	for (auto pCollider : m_Collidables)
	{
		if (pCollider && pCollider->Get_CompActive())
		{
			iActiveCount++;
			if (pCollider->IsTrigger()) iTriggerCount++;
			if (pCollider->IsColliding()) iCollidingCount++;
		}
	}

	ImGui::Text("Active: %d | Inactive: %d", iActiveCount, m_Colliders.size() - iActiveCount);
	ImGui::Text("Triggers: %d", iTriggerCount);
	ImGui::Text("Currently Colliding: %d", iCollidingCount);

	ImGui::Separator();

	 콜라이더 리스트
	if (ImGui::CollapsingHeader("Collider List"))
	{
		for (size_t i = 0; i < m_Colliders.size(); ++i)
		{
			auto pCollider = m_Colliders[i];
			if (!pCollider) continue;

			ImGui::PushID(i);
			_bool bActive = pCollider->Get_CompActive();

			if (ImGui::Checkbox("##Active", &bActive))
			{
				pCollider->Set_CompActive(bActive);
			}

			ImGui::SameLine();
			string ownerName = pCollider->Get_Owner() ?
				pCollider->Get_Owner()->Get_InstanceName() : "No Owner";

			if (pCollider->IsColliding())
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "[HIT] %s", ownerName.c_str());
			}
			else
			{
				ImGui::Text("%s", ownerName.c_str());
			}

			ImGui::PopID();
		}
	}

	ImGui::End();*/
}

void CCollisionSystem::Process_Contact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	if (pairHeader.flags & (PxContactPairHeaderFlag::eREMOVED_ACTOR_0 | PxContactPairHeaderFlag::eREMOVED_ACTOR_1))
		return;

	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.flags & (PxContactPairFlag::eREMOVED_SHAPE_0 | PxContactPairFlag::eREMOVED_SHAPE_1))
			continue;

		if (!cp.shapes[0] || !cp.shapes[1]) continue;
		if (!cp.shapes[0]->userData || !cp.shapes[1]->userData) continue;

		auto pColA = static_cast<ICollidable*>(cp.shapes[0]->userData);
		auto pColB = static_cast<ICollidable*>(cp.shapes[1]->userData);

		if (!pColA || !pColB) continue;
		if (!pColA->Get_CompActive() || !pColB->Get_CompActive()) continue;

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			pColA->OnCollisionEnter(pColB);
			pColB->OnCollisionEnter(pColA);
		}
		else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			pColA->OnCollisionExit(pColB);
			pColB->OnCollisionExit(pColA);
		}
	}
}

void CCollisionSystem::Process_Trigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		if (!pairs[i].triggerShape || !pairs[i].otherShape) continue;
		if (!pairs[i].triggerShape->userData || !pairs[i].otherShape->userData) continue;

		auto pTrigger = static_cast<ICollidable*>(pairs[i].triggerShape->userData);
		auto pOther = static_cast<ICollidable*>(pairs[i].otherShape->userData);

		if (!pTrigger || !pOther) continue;
		if (!pTrigger->Get_CompActive() || !pOther->Get_CompActive()) continue;

		if (static_cast<CCollider*>(pTrigger) != nullptr)
		{
			CCollider* pTriggerCol = static_cast<CCollider*>(pTrigger);
			if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				pTriggerCol->OnTriggerEnter(pOther);
			}
			else if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				pTriggerCol->OnTriggerExit(pOther);
			}
		}
	}
}

void CCollisionSystem::Process_Stay()
{
	for (auto pCollidable : m_Collidables)
	{
		if (!pCollidable || !pCollidable->Get_CompActive()) continue;

		for (auto pOther : pCollidable->Get_Collisions())
		{
			if (pOther && pOther->Get_CompActive())
			{
				pCollidable->OnCollisionStay(pOther);
			}
		}
	}
}

void CCollisionSystem::Process_Exit()
{
	for (auto pCollidable : m_Collidables)
	{
		if (!pCollidable || !pCollidable->Get_CompActive()) continue;

		auto& collisions = const_cast<unordered_set<ICollidable*>&>(pCollidable->Get_Collisions());
		for (auto it = collisions.begin(); it != collisions.end();)
		{
			ICollidable* pOther = *it;
			if (!pOther || !pOther->Get_CompActive())
			{
				it = collisions.erase(it);
				if (pOther)
				{
					pCollidable->OnCollisionExit(pOther);
					pOther->OnCollisionExit(pCollidable);
				}
			}
			else
			{
				++it;
			}
		}
	}
}

_int CCollisionSystem::RegisterCollidable(ICollidable* pCollidable, _int Index)
{
	m_Collidables.push_back(pCollidable);
	return m_Collidables.size() - 1;
}

void CCollisionSystem::UnRegisterCollidable(ICollidable* pCollidable, _int Index)
{
	for (size_t i = 0; i < m_Collidables.size(); ++i)
	{
		if (m_Collidables[i] == pCollidable)
		{
			// 제거할 대상을 마지막 요소와 교체하고 pop_back
			if (i != m_Collidables.size() - 1)
			{
				swap(m_Collidables[i], m_Collidables.back());
			}
			m_Collidables.pop_back();
			return;
		}
	}
}

#ifdef _DEBUG

void CCollisionSystem::Render_Debug()
{
	if (m_Collidables.empty()) return;

	auto camMgr = CGameInstance::GetInstance()->Get_CameraMgr();
	_matrix viewMat = XMLoadFloat4x4(camMgr->Get_ViewMatrix());
	_matrix projMat = XMLoadFloat4x4(camMgr->Get_ProjMatrix());

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(viewMat);
	m_pEffect->SetProjection(projMat);

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	XMVECTOR vColor;
	for (auto& pCollidable : m_Collidables)
	{
		if (pCollidable && pCollidable->Get_CompActive())
		{
			if (dynamic_cast<CCollider*>(pCollidable) != nullptr)
			{
				vColor = pCollidable->IsColliding() ? Colors::Red : Colors::Green;
			}
			else if(dynamic_cast<CCharacterController*>(pCollidable) != nullptr)
			{
				vColor = Colors::Yellow;
				if (dynamic_cast<CCharacterController*>(pCollidable)->Is_Grounded())
					vColor = pCollidable->IsColliding() ? Colors::Orange : Colors::Cyan;
			}
			pCollidable->Render(m_pBatch, vColor);
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
	m_Collidables.clear();
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