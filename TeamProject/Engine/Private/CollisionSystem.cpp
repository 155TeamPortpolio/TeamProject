#include "Engine_Defines.h"
#include "CollisionSystem.h"
#include "GameInstance.h"
#include "ICameraService.h"
#include "Collider.h"
#include "CharacterController.h"

#pragma region CALLBACK_FUNCTION
// Collider
void CCollisionSystem::CPhysXEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	if (m_pOwner) m_pOwner->Process_Contact(pairHeader, pairs, nbPairs);
}

void CCollisionSystem::CPhysXEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	if (m_pOwner) m_pOwner->Process_Trigger(pairs, count);
}
//CCT
void CCollisionSystem::CCCTHitCallback::onShapeHit(const PxControllerShapeHit& hit)
{
	if (m_pOwner) m_pOwner->Process_CCT_ShapeHit(hit);
}

void CCollisionSystem::CCCTHitCallback::onControllerHit(const PxControllersHit& hit)
{
	if (m_pOwner) m_pOwner->Process_CCT_ControllerHit(hit);
}

void CCollisionSystem::CCCTHitCallback::onObstacleHit(const PxControllerObstacleHit& hit)
{
	if (m_pOwner) m_pOwner->Process_CCT_ObstacleHit(hit);
}
#pragma endregion

CCollisionSystem::CCollisionSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice{ pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}

HRESULT CCollisionSystem::Initialize()
{
//#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t      iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
//#endif

	m_Collidables.reserve(1000);

	// 프록시 콜백
	m_pPhysXCallback = new CPhysXEventCallback(this);
	m_pCCTCallback = new CCCTHitCallback(this);
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
#ifdef USE_MULTITHREAD_PHYSICS
	lock_guard<recursive_mutex> lock(m_SlotMutex);
#endif

	for (auto& slot : m_Collidables)
	{
		if (!slot.pCollidable) continue;

		if (slot.pCollidable->Get_CompActive())
		{
			if (slot.eState == COLLIDABLE_SLOT::STATE::INACTIVE)
			{	// 활성화 시 충돌목록 초기화
				slot.eState = COLLIDABLE_SLOT::STATE::ACTIVE;
				slot.pCollidable->Get_CurrentCollisions().clear();
				slot.pCollidable->Get_PreviousCollisions().clear();
			}
		}
		else
		{
			if (slot.eState == COLLIDABLE_SLOT::STATE::ACTIVE)
			{	// 비활성화 시 충돌목록 초기화
				slot.eState = COLLIDABLE_SLOT::STATE::INACTIVE;
				slot.pCollidable->Get_CurrentCollisions().clear();
				slot.pCollidable->Get_PreviousCollisions().clear();
			}
		}
	}

	for (auto& slot : m_Collidables)
	{
		if (slot.IsActive() && slot.pCollidable->Get_CompActive())
		{
			slot.pCollidable->Update_Collisions();
		}
	}
}

void CCollisionSystem::Late_Update(_float dt)
{
#ifdef USE_MULTITHREAD_PHYSICS
	lock_guard<recursive_mutex> lock(m_SlotMutex);
#endif
	Process_CollisionEvents();
	Remove_DeactiveSlots();
	Clean_DeadSlots();
}

void CCollisionSystem::Render_GUI()
{
	ImGui::Begin("Collision System");

	_uint iTotalCount = 0;
	_uint iActiveCount = 0;
	_uint iInactiveCount = 0;
	_uint iDeadCount = 0;
	_uint iTriggerCount = 0;
	_uint iCollidingCount = 0;
	_uint iColliderCount = 0;
	_uint iCCTCount = 0;

	for (const auto& slot : m_Collidables)
	{
		if (slot.pCollidable)
			iTotalCount++;

		if (slot.eState == COLLIDABLE_SLOT::STATE::ACTIVE)
			iActiveCount++;
		else if (slot.eState == COLLIDABLE_SLOT::STATE::INACTIVE)
			iInactiveCount++;
		else if (slot.eState == COLLIDABLE_SLOT::STATE::DEAD)
			iDeadCount++;

		if (slot.IsValid() && slot.pCollidable->Get_CompActive())
		{
			if (slot.pCollidable->IsColliding())
				iCollidingCount++;

			if (dynamic_cast<CCollider*>(slot.pCollidable))
			{
				iColliderCount++;
				CCollider* pCol = static_cast<CCollider*>(slot.pCollidable);
				if (pCol->IsTrigger()) iTriggerCount++;
			}
			else if (dynamic_cast<CCharacterController*>(slot.pCollidable))
			{
				iCCTCount++;
			}
		}
	}

	ImGui::Text("Total Collidables: %d", iTotalCount);
	ImGui::Text("Active: %d | Inactive: %d", iActiveCount, iTotalCount - iActiveCount);
	ImGui::Text("Colliders: %d | CCT: %d", iColliderCount, iCCTCount);
	ImGui::Text("Triggers: %d", iTriggerCount);
	ImGui::Text("Currently Colliding: %d", iCollidingCount);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Collidable List"))
	{
		for (size_t i = 0; i < m_Collidables.size(); ++i)
		{
			auto& slot = m_Collidables[i];
			if (!slot.pCollidable) continue;

			ImGui::PushID(i);

			_bool bActive = slot.pCollidable->Get_CompActive();

			if (ImGui::Checkbox("##Active", &bActive))
			{
				slot.pCollidable->Set_CompActive(bActive);
			}

			ImGui::SameLine();

			const char* typePrefix = "";
			if (dynamic_cast<CCollider*>(slot.pCollidable))
			{
				CCollider* pCol = static_cast<CCollider*>(slot.pCollidable);
				typePrefix = pCol->IsTrigger() ? "[TRG]" : "[COL]";
			}
			else if (dynamic_cast<CCharacterController*>(slot.pCollidable))
			{
				typePrefix = "[CCT]";
			}

			string ownerName = slot.pCollidable->Get_Owner() ?
				slot.pCollidable->Get_Owner()->Get_InstanceName() : "No Owner";

			if (slot.eState == COLLIDABLE_SLOT::STATE::DEAD)
			{
				ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), "%s [DEAD] %s (Gen:%d)",
					typePrefix, ownerName.c_str(), slot.iGeneration);
			}
			else if (slot.eState == COLLIDABLE_SLOT::STATE::INACTIVE)
			{
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.f, 1), "%s [INACTIVE] %s (Gen:%d)",
					typePrefix, ownerName.c_str(), slot.iGeneration);
			}
			else if (slot.pCollidable->IsColliding())
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s [HIT] %s (Gen:%d)",
					typePrefix, ownerName.c_str(), slot.iGeneration);
			}
			else
			{
				ImGui::Text("%s %s (Gen:%d)", typePrefix, ownerName.c_str(), slot.iGeneration);
			}

			ImGui::PopID();
		}
	}

	ImGui::End();
}

void CCollisionSystem::Process_Contact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
#ifdef USE_MULTITHREAD_PHYSICS
	lock_guard<recursive_mutex> lock(m_SlotMutex);
#endif
	if (pairHeader.flags & (PxContactPairHeaderFlag::eREMOVED_ACTOR_0 | PxContactPairHeaderFlag::eREMOVED_ACTOR_1))
		return;

	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.flags & (PxContactPairFlag::eREMOVED_SHAPE_0 | PxContactPairFlag::eREMOVED_SHAPE_1))
			continue;

		if (!cp.shapes[0] || !cp.shapes[1]) continue;

		ICollidable* pColA = nullptr;
		ICollidable* pColB = nullptr;
		_int idxA = -1;
		_int idxB = -1;

		// Shape A 검증
		if (cp.shapes[0]->userData)
		{
			pColA = static_cast<ICollidable*>(cp.shapes[0]->userData);
			idxA = pColA->Get_SlotIndex();
			if (!Is_SlotActive(idxA))
				pColA = nullptr;
		}
		else if (pairHeader.actors[0] && pairHeader.actors[0]->userData)
		{
			pColA = Get_Collidable_Actor(pairHeader.actors[0]);
			if (pColA)
			{
				idxA = pColA->Get_SlotIndex();
				if (!Is_SlotActive(idxA))
					pColA = nullptr;
			}
		}

		// Shape B 검증
		if (cp.shapes[1]->userData)
		{
			pColB = static_cast<ICollidable*>(cp.shapes[1]->userData);
			idxB = pColB->Get_SlotIndex();
			if (!Is_SlotActive(idxB))
				pColB = nullptr;
		}
		else if (pairHeader.actors[1] && pairHeader.actors[1]->userData)
		{
			pColB = Get_Collidable_Actor(pairHeader.actors[1]);
			if (pColB)
			{
				idxB = pColB->Get_SlotIndex();
				if (!Is_SlotActive(idxB))
					pColB = nullptr;
			}
		}

		if (!pColA || !pColB) continue;
		if (!pColA->Get_CompActive() || !pColB->Get_CompActive()) continue;
		if (pColA->Get_Owner() && pColB->Get_Owner() &&
			pColA->Get_Owner() == pColB->Get_Owner())
			continue;

		auto& currentA = pColA->Get_CurrentCollisions();
		auto& currentB = pColB->Get_CurrentCollisions();

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			// A -> B Enter
			if (currentA.find(pColB) == currentA.end())
			{
				currentA.insert(pColB);
				pColA->OnCollisionEnter(pColB);

				// Enter 후 재검증 (Enter 내부에서 삭제 가능성)
				if (!Is_SlotActive(idxA) || !Is_SlotActive(idxB))
					continue;
			}

			// 재검증
			if (!Is_SlotActive(idxA) || !Is_SlotActive(idxB))
				continue;

			// B -> A Enter
			if (currentB.find(pColA) == currentB.end())
			{
				currentB.insert(pColA);
				pColB->OnCollisionEnter(pColA);
			}
		}
		else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			currentA.insert(pColB);
			currentB.insert(pColA);
		}
		else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			currentA.erase(pColB);
			currentB.erase(pColA);
		}
	}
}

void CCollisionSystem::Process_Trigger(PxTriggerPair* pairs, PxU32 count)
{
#ifdef USE_MULTITHREAD_PHYSICS
	lock_guard<recursive_mutex> lock(m_SlotMutex);
#endif
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		if (!pairs[i].triggerShape || !pairs[i].otherShape) continue;

		ICollidable* pTrigger = nullptr;
		ICollidable* pOther = nullptr;
		_int idxTrigger = -1;
		_int idxOther = -1;

		// Trigger Shape 검증
		if (pairs[i].triggerShape->userData)
		{
			pTrigger = static_cast<ICollidable*>(pairs[i].triggerShape->userData);
			idxTrigger = pTrigger->Get_SlotIndex();
			if (!Is_SlotActive(idxTrigger))
				pTrigger = nullptr;
		}

		// Other Shape 검증
		if (pairs[i].otherShape->userData)
		{
			pOther = static_cast<ICollidable*>(pairs[i].otherShape->userData);
			idxOther = pOther->Get_SlotIndex();
			if (!Is_SlotActive(idxOther))
				pOther = nullptr;
		}

		if (!pTrigger || !pOther) continue;
		if (!pTrigger->Get_CompActive() || !pOther->Get_CompActive()) continue;

		if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			pTrigger->OnTriggerEnter(pOther);

			// Enter 후 재검증
			if (!Is_SlotActive(idxTrigger) || !Is_SlotActive(idxOther))
				continue;

			pOther->OnTriggerEnter(pTrigger);
		}
		else if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			pTrigger->OnTriggerExit(pOther);

			// Exit 후 재검증
			if (!Is_SlotActive(idxTrigger) || !Is_SlotActive(idxOther))
				continue;

			pOther->OnTriggerExit(pTrigger);
		}
	}
}

void CCollisionSystem::Process_CCT_ShapeHit(const PxControllerShapeHit& hit)
{
#ifdef USE_MULTITHREAD_PHYSICS
	lock_guard<recursive_mutex> lock(m_SlotMutex);
#endif
	if (!hit.controller || !hit.shape) return;

	PxRigidDynamic* pCCTActor = hit.controller->getActor();
	if (!pCCTActor || !pCCTActor->userData) return;

	CGameObject* pCCTOwner = static_cast<CGameObject*>(pCCTActor->userData);
	ICollidable* pCCT = pCCTOwner ? pCCTOwner->Get_Component<CCharacterController>() : nullptr;

	// CCT 슬롯 검증
	_int idxCCT = -1;
	if (pCCT)
	{
		idxCCT = pCCT->Get_SlotIndex();
		if (!Is_SlotActive(idxCCT))
			return;
	}
	else
	{
		return;
	}

	ICollidable* pOther = Get_Collidable_Shape(hit.shape, hit.actor);

	// Other 슬롯 검증
	_int idxOther = -1;
	if (pOther)
	{
		idxOther = pOther->Get_SlotIndex();
		if (!Is_SlotActive(idxOther))
			return;
	}
	else
	{
		return;
	}

	// Trigger 처리
	CCollider* pCollider = dynamic_cast<CCollider*>(pOther);
	if (pCollider && pCollider->IsTrigger())
	{
		auto& cctPrevious = pCCT->Get_PreviousCollisions();

		if (cctPrevious.find(pOther) == cctPrevious.end())
		{
			pCCT->OnTriggerEnter(pOther);

			// Enter 후 재검증
			if (!Is_SlotActive(idxCCT) || !Is_SlotActive(idxOther))
				return;

			pOther->OnTriggerEnter(pCCT);
		}

		pCCT->Get_CurrentCollisions().insert(pOther);
		pOther->Get_CurrentCollisions().insert(pCCT);

		return;
	}

	// 일반 Collision 처리
	auto& cctCurrent = pCCT->Get_CurrentCollisions();
	auto& cctPrevious = pCCT->Get_PreviousCollisions();

	cctCurrent.insert(pOther);

	if (cctPrevious.find(pOther) == cctPrevious.end())
	{
		pCCT->OnCollisionEnter(pOther);

		// Enter 후 재검증
		if (!Is_SlotActive(idxCCT) || !Is_SlotActive(idxOther))
			return;

		pOther->OnCollisionEnter(pCCT);
	}

	// 재검증
	if (!Is_SlotActive(idxOther))
		return;

	pOther->Get_CurrentCollisions().insert(pCCT);

	// 재검증
	if (!Is_SlotActive(idxCCT))
		return;

	static_cast<CCharacterController*>(pCCT)->Process_Response(hit);
}

void CCollisionSystem::Process_CCT_ControllerHit(const PxControllersHit& hit)
{
#ifdef USE_MULTITHREAD_PHYSICS
	lock_guard<recursive_mutex> lock(m_SlotMutex);
#endif
	if (!hit.controller || !hit.other) return;

	// 첫 번째 CCT
	PxRigidDynamic* pCCTActor1 = hit.controller->getActor();
	CGameObject* pOwner1 = pCCTActor1 && pCCTActor1->userData ?
		static_cast<CGameObject*>(pCCTActor1->userData) : nullptr;
	ICollidable* pCCT1 = pOwner1 ? pOwner1->Get_Component<CCharacterController>() : nullptr;

	// 두 번째 CCT
	PxRigidDynamic* pCCTActor2 = hit.other->getActor();
	CGameObject* pOwner2 = pCCTActor2 && pCCTActor2->userData ?
		static_cast<CGameObject*>(pCCTActor2->userData) : nullptr;
	ICollidable* pCCT2 = pOwner2 ? pOwner2->Get_Component<CCharacterController>() : nullptr;

	// 슬롯 검증
	_int idx1 = -1;
	_int idx2 = -1;

	if (pCCT1)
	{
		idx1 = pCCT1->Get_SlotIndex();
		if (!Is_SlotActive(idx1))
			return;
	}
	else
	{
		return;
	}

	if (pCCT2)
	{
		idx2 = pCCT2->Get_SlotIndex();
		if (!Is_SlotActive(idx2))
			return;
	}
	else
	{
		return;
	}

	auto& current1 = pCCT1->Get_CurrentCollisions();
	auto& current2 = pCCT2->Get_CurrentCollisions();
	auto& previous1 = pCCT1->Get_PreviousCollisions();

	current1.insert(pCCT2);

	if (previous1.find(pCCT2) == previous1.end())
	{
		pCCT1->OnCollisionEnter(pCCT2);

		// Enter 후 재검증
		if (!Is_SlotActive(idx1) || !Is_SlotActive(idx2))
			return;

		pCCT2->OnCollisionEnter(pCCT1);
	}

	// 재검증
	if (!Is_SlotActive(idx2))
		return;

	current2.insert(pCCT1);
}

void CCollisionSystem::Process_CCT_ObstacleHit(const PxControllerObstacleHit& hit)
{
#ifdef USE_MULTITHREAD_PHYSICS
	lock_guard<recursive_mutex> lock(m_SlotMutex);
#endif
	// 잘 안씀
	if (!hit.controller) return;

	PxRigidDynamic* pCCTActor = hit.controller->getActor();
	if (!pCCTActor || !pCCTActor->userData) return;

	CGameObject* pCCTOwner = static_cast<CGameObject*>(pCCTActor->userData);
	CCharacterController* pCCT = pCCTOwner ?
		pCCTOwner->Get_Component<CCharacterController>() : nullptr;

	// 슬롯 검증
	if (pCCT)
	{
		_int idx = pCCT->Get_SlotIndex();
		if (Is_SlotActive(idx))
		{
			// Obstacle 충돌 처리 (슬라이딩, 사운드 등)
		}
	}
}

void CCollisionSystem::Process_CollisionEvents()
{
	for (size_t slotIdx = 0; slotIdx < m_Collidables.size(); ++slotIdx)
	{
		auto& slot = m_Collidables[slotIdx];
		if (!slot.IsActive()) continue;

		auto pCollidable = slot.pCollidable;
		if (!pCollidable->Get_Owner()) continue;

		auto& current = pCollidable->Get_CurrentCollisions();
		auto& previous = pCollidable->Get_PreviousCollisions();
		// Exit 이벤트 처리
		for (auto it = previous.begin(); it != previous.end(); ++it)
		{
			auto pOther = *it;
			// 슬롯 검증으로 해제된 포인터 필터링
			if (!pOther) continue;

			_int otherIdx = pOther->Get_SlotIndex();
			if (otherIdx < 0 || otherIdx >= static_cast<_int>(m_Collidables.size()))
				continue;

			const auto& otherSlot = m_Collidables[otherIdx];

			if (otherSlot.pCollidable != pOther) continue;
			if (otherSlot.iGeneration != pOther->Get_SlotGeneration()) continue;
			if (!otherSlot.IsValid()) continue;
			if (!pOther->Get_Owner()) continue;
			// Current에 없으면 Exit
			if (!otherSlot.IsActive() || current.find(pOther) == current.end())
			{
				CCollider* pCollider = dynamic_cast<CCollider*>(pOther);
				if (pCollider && pCollider->IsTrigger())
				{
					pCollidable->OnTriggerExit(pOther);
				}
				else
				{
					pCollidable->OnCollisionExit(pOther);
				}

				if (!slot.IsActive() || !pCollidable->Get_Owner())
					break;
			}
		}

		if (!slot.IsActive() || !pCollidable->Get_Owner()) continue;
		// Stay 이벤트 처리
		for (auto it = current.begin(); it != current.end(); ++it)
		{
			auto pOther = *it;
			// 슬롯 검증으로 해제된 포인터 필터링
			if (!pOther) continue;

			_int otherIdx = pOther->Get_SlotIndex();
			if (otherIdx < 0 || otherIdx >= static_cast<_int>(m_Collidables.size()))
				continue;

			const auto& otherSlot = m_Collidables[otherIdx];

			if (otherSlot.pCollidable != pOther) continue;
			if (otherSlot.iGeneration != pOther->Get_SlotGeneration()) continue;
			if (!otherSlot.IsActive()) continue;
			if (!pOther->Get_Owner()) continue;

			// Previous에도 있으면 Stay
			if (previous.find(pOther) != previous.end())
			{
				CCollider* pCollider = dynamic_cast<CCollider*>(pOther);
				if (!pCollider || !pCollider->IsTrigger())
				{
					pCollidable->OnCollisionStay(pOther);

					if (!slot.IsActive() || !pCollidable->Get_Owner())
						break;
				}
			}
		}
	}
}

void CCollisionSystem::Remove_DeactiveSlots()
{
	// DEAD 또는 INACTIVE 상태인 Collidable 수집
	unordered_set<ICollidable*> deactiveCollidables;

	for (const auto& slot : m_Collidables)
	{
		// DEAD이거나 INACTIVE인 경우
		if (slot.pCollidable && !slot.IsActive())
		{
			deactiveCollidables.insert(slot.pCollidable);
		}
	}

	if (deactiveCollidables.empty())
		return;

	// 모든 ACTIVE Collidable의 충돌 목록에서 제거
	for (auto& slot : m_Collidables)
	{
		if (!slot.IsActive()) continue;

		auto& current = slot.pCollidable->Get_CurrentCollisions();
		auto& previous = slot.pCollidable->Get_PreviousCollisions();

		for (auto pInvalid : deactiveCollidables)
		{
			current.erase(pInvalid);
			previous.erase(pInvalid);
		}
	}
}

void CCollisionSystem::Clean_DeadSlots()
{
	for (auto& slot : m_Collidables)
	{
		if (slot.eState == COLLIDABLE_SLOT::STATE::DEAD)
		{
			slot.pCollidable = nullptr;
			slot.iGeneration++;
		}
	}
}

_bool CCollisionSystem::Is_SlotActive(_int iIndex) const
{
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_Collidables.size()))
		return false;

	const auto& slot = m_Collidables[iIndex];

	return slot.IsActive() &&
		slot.pCollidable &&
		slot.pCollidable->Get_CompActive();
}

ICollidable* CCollisionSystem::Get_Collidable_Actor(PxRigidActor* pActor)
{
	if (!pActor || !pActor->userData) return nullptr;

	CGameObject* pOwner = static_cast<CGameObject*>(pActor->userData);
	if (!pOwner) return nullptr;

	ICollidable* pCollidable = pOwner->Get_Component<CCharacterController>();
	if (!pCollidable)
		pCollidable = pOwner->Get_Component<CCollider>();

	return pCollidable;
}

ICollidable* CCollisionSystem::Get_Collidable_Shape(PxShape* pShape, PxRigidActor* pActor)
{
	if (!pShape) return nullptr;

	// Shape의 userData 먼저 확인
	if (pShape->userData)
		return static_cast<ICollidable*>(pShape->userData);

	// Actor에서 GameObject 찾기
	if (pActor && pActor->userData)
	{
		CGameObject* pOwner = static_cast<CGameObject*>(pActor->userData);
		if (pOwner)
		{
			ICollidable* pCol = pOwner->Get_Component<CCollider>();
			if (!pCol)
				pCol = pOwner->Get_Component<CCharacterController>();
			return pCol;
		}
	}

	return nullptr;
}

_int CCollisionSystem::RegisterCollidable(ICollidable* pCollidable, _int Index)
{
#ifdef USE_MULTITHREAD_PHYSICS
	lock_guard<recursive_mutex> lock(m_SlotMutex);
#endif
	if (m_Collidables.capacity() < m_Collidables.size() + 1)
	{
		m_Collidables.reserve(m_Collidables.capacity() * 2);
	}

	for (size_t i = 0; i < m_Collidables.size(); ++i)
	{
		if (m_Collidables[i].pCollidable == nullptr)
		{
			m_Collidables[i].pCollidable = pCollidable;
			m_Collidables[i].eState = COLLIDABLE_SLOT::STATE::ACTIVE;
			m_Collidables[i].iGeneration++;

			pCollidable->Set_SlotInfo(static_cast<_int>(i), m_Collidables[i].iGeneration);

			return static_cast<_int>(i);
		}
	}

	COLLIDABLE_SLOT slot;
	slot.pCollidable = pCollidable;
	slot.eState = COLLIDABLE_SLOT::STATE::ACTIVE;
	slot.iGeneration = 1;
	m_Collidables.push_back(slot);

	_int newIdx = static_cast<_int>(m_Collidables.size() - 1);

	pCollidable->Set_SlotInfo(newIdx, slot.iGeneration);

	return newIdx;
}

void CCollisionSystem::UnRegisterCollidable(ICollidable* pCollidable, _int Index)
{
#ifdef USE_MULTITHREAD_PHYSICS
	lock_guard<recursive_mutex> lock(m_SlotMutex);
#endif
	if (!pCollidable)
		return;

	_int idx = pCollidable->Get_SlotIndex();
	if (idx >= 0 && idx < static_cast<_int>(m_Collidables.size()))
	{
		if (m_Collidables[idx].pCollidable == pCollidable)
		{
			m_Collidables[idx].eState = COLLIDABLE_SLOT::STATE::DEAD;
		}
	}
}

//#ifdef _DEBUG
void CCollisionSystem::Render_Debug()
{
	if (!m_bRender)	return;
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
	for (const auto& slot : m_Collidables)
	{
		if (slot.IsValid() && slot.pCollidable->Get_CompActive())
		{
			if (dynamic_cast<CCollider*>(slot.pCollidable))
			{
				vColor = slot.pCollidable->IsColliding() ? Colors::Red : Colors::Green;
			}
			else if (dynamic_cast<CCharacterController*>(slot.pCollidable))
			{
				vColor = slot.pCollidable->IsColliding() ? Colors::Orange : Colors::Cyan;
			}
			slot.pCollidable->Render(m_pBatch, vColor);
		}
	}

	m_pBatch->End();
}
//#endif 

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

	if (m_pCCTCallback)
	{
		delete m_pCCTCallback;
		m_pCCTCallback = nullptr;
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

//#ifdef _DEBUG
	m_Collidables.clear();
	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pEffect);
	Safe_Delete(m_pBatch);
//#endif 

	__super::Free();
}
