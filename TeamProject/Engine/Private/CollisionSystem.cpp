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
	for (auto it = m_Collidables.begin(); it != m_Collidables.end();)
	{
		if ((*it) == nullptr) // 이미 해제된 경우
			it = m_Collidables.erase(it);
		else
			++it;
	}
	// 이전 프레임 충돌 백업
	for (auto pCollidable : m_Collidables)
	{
		if (pCollidable && pCollidable->Get_CompActive())
		{
			pCollidable->Update_Collisions();	// CCT : Current.clear()
		}
	}
}

void CCollisionSystem::Late_Update(_float dt)
{
	Process_CollisionEvents();
}

void CCollisionSystem::Render_GUI()
{
	ImGui::Begin("Collision System");

	ImGui::Text("Total Collidables: %d", m_Collidables.size());

	_uint iActiveCount = 0;
	_uint iTriggerCount = 0;
	_uint iCollidingCount = 0;
	_uint iColliderCount = 0;
	_uint iCCTCount = 0;

	for (auto pCollidable : m_Collidables)
	{
		if (pCollidable && pCollidable->Get_CompActive())
		{
			iActiveCount++;
			if (pCollidable->IsColliding())
				iCollidingCount++;

			if (dynamic_cast<CCollider*>(pCollidable))
			{
				iColliderCount++;
				CCollider* pCol = static_cast<CCollider*>(pCollidable);
				if (pCol->IsTrigger()) iTriggerCount++;
			}
			else if (dynamic_cast<CCharacterController*>(pCollidable))
			{
				iCCTCount++;
			}
		}
	}

	ImGui::Text("Active: %d | Inactive: %d", iActiveCount, m_Collidables.size() - iActiveCount);
	ImGui::Text("Colliders: %d | CCT: %d", iColliderCount, iCCTCount);
	ImGui::Text("Triggers: %d", iTriggerCount);
	ImGui::Text("Currently Colliding: %d", iCollidingCount);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Collidable List"))
	{
		for (size_t i = 0; i < m_Collidables.size(); ++i)
		{
			auto pCollidable = m_Collidables[i];
			if (!pCollidable) continue;

			ImGui::PushID(i);
			_bool bActive = pCollidable->Get_CompActive();

			if (ImGui::Checkbox("##Active", &bActive))
			{
				pCollidable->Set_CompActive(bActive);
			}

			ImGui::SameLine();

			const char* typePrefix = "";
			if (dynamic_cast<CCollider*>(pCollidable))
			{
				CCollider* pCol = static_cast<CCollider*>(pCollidable);
				typePrefix = pCol->IsTrigger() ? "[TRG]" : "[COL]";
			}
			else if (dynamic_cast<CCharacterController*>(pCollidable))
			{
				typePrefix = "[CCT]";
			}

			string ownerName = pCollidable->Get_Owner() ?
				pCollidable->Get_Owner()->Get_InstanceName() : "No Owner";

			if (pCollidable->IsColliding())
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s [HIT] %s", typePrefix, ownerName.c_str());
			}
			else
			{
				ImGui::Text("%s %s", typePrefix, ownerName.c_str());
			}

			ImGui::PopID();
		}
	}

	ImGui::End();
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

		ICollidable* pColA = nullptr;
		ICollidable* pColB = nullptr;

		if (cp.shapes[0]->userData)
			pColA = static_cast<ICollidable*>(cp.shapes[0]->userData);
		else if (pairHeader.actors[0] && pairHeader.actors[0]->userData)
			pColA = Get_Collidable_Actor(pairHeader.actors[0]);

		if (cp.shapes[1]->userData)
			pColB = static_cast<ICollidable*>(cp.shapes[1]->userData);
		else if (pairHeader.actors[1] && pairHeader.actors[1]->userData)
			pColB = Get_Collidable_Actor(pairHeader.actors[1]);

		if (!pColA || !pColB) continue;
		if (!pColA->Get_CompActive() || !pColB->Get_CompActive()) continue;

		auto& currentA = pColA->Get_CurrentCollisions();
		auto& currentB = pColB->Get_CurrentCollisions();

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			// Enter: Current에 추가 + Enter 이벤트
			if (currentA.find(pColB) == currentA.end())
			{
				currentA.insert(pColB);
				pColA->OnCollisionEnter(pColB);
			}

			if (currentB.find(pColA) == currentB.end())
			{
				currentB.insert(pColA);
				pColB->OnCollisionEnter(pColA);
			}
		}
		else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			// Stay: Current에 유지 (이미 있으면 그대로, 없으면 추가)
			currentA.insert(pColB);
			currentB.insert(pColA);
		}
		else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			// Exit: Current에서 제거
			currentA.erase(pColB);
			currentB.erase(pColA);
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

		ICollidable* pTrigger = nullptr;
		ICollidable* pOther = nullptr;

		if (pairs[i].triggerShape->userData)
			pTrigger = static_cast<ICollidable*>(pairs[i].triggerShape->userData);

		if (pairs[i].otherShape->userData)
			pOther = static_cast<ICollidable*>(pairs[i].otherShape->userData);

		if (!pTrigger || !pOther) continue;
		if (!pTrigger->Get_CompActive() || !pOther->Get_CompActive()) continue;

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

void CCollisionSystem::Process_CCT_ShapeHit(const PxControllerShapeHit& hit)
{
	if (!hit.controller || !hit.shape) return;

	PxRigidDynamic* pCCTActor = hit.controller->getActor();
	if (!pCCTActor || !pCCTActor->userData) return;

	CGameObject* pCCTOwner = static_cast<CGameObject*>(pCCTActor->userData);
	ICollidable* pCCT = pCCTOwner ? pCCTOwner->Get_Component<CCharacterController>() : nullptr;
	if (!pCCT || !pCCT->Get_CompActive()) return;

	ICollidable* pOther = Get_Collidable_Shape(hit.shape, hit.actor);
	if (!pOther || !pOther->Get_CompActive()) return;

	// 트리거 Collision 처리
	CCollider* pCollider = dynamic_cast<CCollider*>(pOther);
	if (pCollider && pCollider->IsTrigger())
	{
		// Trigger 처리
		auto& cctPrevious = pCCT->Get_PreviousCollisions();

		// Enter 체크 (Previous에 없으면)
		if (cctPrevious.find(pOther) == cctPrevious.end())
		{
			pCCT->OnTriggerEnter(pOther);
			pOther->OnTriggerEnter(pCCT);
		}

		// Current에는 추가 (Exit 판정용)
		pCCT->Get_CurrentCollisions().insert(pOther);
		pOther->Get_CurrentCollisions().insert(pCCT);

		// 물리 반응은 하지 않음
		return;
	}

	// 일반 Collision 처리
	auto& cctCurrent = pCCT->Get_CurrentCollisions();
	auto& cctPrevious = pCCT->Get_PreviousCollisions();

	cctCurrent.insert(pOther);

	// Enter 체크 (Previous에 없으면 Enter)
	if (cctPrevious.find(pOther) == cctPrevious.end())
	{
		pCCT->OnCollisionEnter(pOther);
		pOther->OnCollisionEnter(pCCT);
	}

	// Collider 쪽에도 추가
	pOther->Get_CurrentCollisions().insert(pCCT);

	static_cast<CCharacterController*>(pCCT)->Process_Response(hit);
}

void CCollisionSystem::Process_CCT_ControllerHit(const PxControllersHit& hit)
{
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

	if (!pCCT1 || !pCCT2) return;
	if (!pCCT1->Get_CompActive() || !pCCT2->Get_CompActive()) return;

	auto& current1 = pCCT1->Get_CurrentCollisions();
	auto& current2 = pCCT2->Get_CurrentCollisions();
	auto& previous1 = pCCT1->Get_PreviousCollisions();

	current1.insert(pCCT2);

	if (previous1.find(pCCT2) == previous1.end())
	{
		pCCT1->OnCollisionEnter(pCCT2);
		pCCT2->OnCollisionEnter(pCCT1);
	}

	// CCT2 Current에도 추가
	current2.insert(pCCT1);
}

void CCollisionSystem::Process_CCT_ObstacleHit(const PxControllerObstacleHit& hit)
{
	// 필요시 구현 (거의 안쓴다고 합니다.)
	// Obstacle : CCT 전용 정적 장애물, Actor없음, 가볍고빠름, 박스만 가능, CCT만 충돌 가능
	if (!hit.controller) return;

	PxRigidDynamic* pCCTActor = hit.controller->getActor();
	if (!pCCTActor || !pCCTActor->userData) return;

	CGameObject* pCCTOwner = static_cast<CGameObject*>(pCCTActor->userData);
	CCharacterController* pCCT = pCCTOwner ?
		pCCTOwner->Get_Component<CCharacterController>() : nullptr;

	if (pCCT && pCCT->Get_CompActive())
	{
		// 슬라이딩, 사운드
	}
}

void CCollisionSystem::Process_CollisionEvents()
{
	for (auto pCollidable : m_Collidables)
	{
		if (!pCollidable || !pCollidable->Get_CompActive()) continue;

		auto& current = pCollidable->Get_CurrentCollisions();
		auto& previous = pCollidable->Get_PreviousCollisions();

		// Exit: 이전엔 있었는데 현재 없는 경우
		vector<ICollidable*> toExit;
		for (auto pOther : previous)
		{
			if (current.find(pOther) == current.end())
			{
				if (pOther)
				{
					_bool bExists = false;
					for (auto Collidable : m_Collidables)
					{
						if (Collidable == pOther)
						{
							bExists = true;
							break;
						}
					}

					if (bExists && pOther->Get_CompActive())
					{
						toExit.push_back(pOther);
					}
				}
			}
		}

		// Exit 이벤트 호출
		for (auto pOther : toExit)
		{
			// 트리거 Collision Exit
			CCollider* pCollider = dynamic_cast<CCollider*>(pOther);
			if (pCollider && pCollider->IsTrigger())
			{
				pCollidable->OnTriggerExit(pOther);
				if (pOther)
				{
					pOther->Get_CurrentCollisions().erase(pCollidable);
				}
			}
			else
			{
				// 일반 Collision Exit
				pCollidable->OnCollisionExit(pOther);
				if (pOther)
				{
					pOther->Get_CurrentCollisions().erase(pCollidable);
				}
			}
		}

		// Stay: 이전과 현재 모두 있는 경우 (Trigger는 Stay 없음)
		for (auto pOther : current)
		{
			if (previous.find(pOther) != previous.end())
			{
				if (pOther && pOther->Get_CompActive())
				{
					pCollidable->OnCollisionStay(pOther);
				}
			}
		}
	}
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
				vColor = pCollidable->IsColliding() ? Colors::Orange : Colors::Cyan;
			}
			pCollidable->Render(m_pBatch, vColor);
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

#ifdef _DEBUG
	m_Collidables.clear();
	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pEffect);
	Safe_Delete(m_pBatch);
#endif 

	__super::Free();
}
