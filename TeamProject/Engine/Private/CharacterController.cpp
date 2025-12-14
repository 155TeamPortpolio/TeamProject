#pragma once
#include "Engine_Defines.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "DebugDraw.h"

void CCharacterController::CCTHitReportProxy::onShapeHit(const PxControllerShapeHit& hit)
{
	if (m_pOwner) m_pOwner->Process_ShapeHit(hit);
}

void CCharacterController::CCTHitReportProxy::onControllerHit(const PxControllersHit& hit)
{
	if (m_pOwner) m_pOwner->Process_ControllerHit(hit);
}

void CCharacterController::CCTHitReportProxy::onObstacleHit(const PxControllerObstacleHit& hit)
{
	if (m_pOwner) m_pOwner->Process_ObstacleHit(hit);
}

CCharacterController::CCharacterController()
{
}

CCharacterController::CCharacterController(const CCharacterController& rhs)
	: ICollidable(rhs)
{
}

void CCharacterController::Set_PlanarVelocity(_fvector vVelocity)
{
	_float3 vIn;
	XMStoreFloat3(&vIn, vVelocity);
	m_vVelocity.x = vIn.x;
	m_vVelocity.z = vIn.z;
}

HRESULT CCharacterController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacterController::Initialize(COMPONENT_DESC* pArg)
{
	m_pPhysicsSystem = CGameInstance::GetInstance()->Get_PhysicsSystem();
	if (!m_pPhysicsSystem) return E_FAIL;

	m_pManager = m_pPhysicsSystem->Get_ControllerManager();
	if (!m_pManager) return E_FAIL;

	m_pOwnerTransform = m_pOwner->Get_Component<CTransform>();
	if (!m_pOwnerTransform)
	{
		MSG_BOX("CCharacterController needs Transform Component!");
		return E_FAIL;
	}

	m_pHitReport = new CCTHitReportProxy(this);
	
	CCT_DESC* pDesc = {nullptr};
	if (pArg)
	{
		pDesc = static_cast<CCT_DESC*>(pArg);
	}

	
	m_pMaterial = m_pPhysicsSystem->Get_Material(pDesc->strMaterialTag);
	PxCapsuleControllerDesc capsuleDesc;
	capsuleDesc.height = pDesc->fHeight;
	capsuleDesc.radius = pDesc->fRadius;
	capsuleDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	capsuleDesc.stepOffset = pDesc->fStepOffset;
	capsuleDesc.material = m_pMaterial;
	capsuleDesc.slopeLimit = cosf(XMConvertToRadians(pDesc->fSlopeLimit));
	capsuleDesc.contactOffset = 0.01f;
	capsuleDesc.upDirection = PxVec3(0, 1, 0);
	capsuleDesc.density = pDesc->fDensity;

	// 초기 위치
	if (pDesc->vPos.x == 0 && pDesc->vPos.y == 0 && pDesc->vPos.z == 0)
	{
		_vector vPos = m_pOwnerTransform->Get_WorldPos();
		_float3 vP; XMStoreFloat3(&vP, vPos);
		capsuleDesc.position = PxExtendedVec3(vP.x, vP.y, vP.z);
	}
	else
	{
		capsuleDesc.position = PxExtendedVec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z);
	}


	capsuleDesc.reportCallback = m_pHitReport;
	capsuleDesc.behaviorCallback = nullptr;
	if (!capsuleDesc.isValid())
	{
		delete m_pHitReport;
		MSG_BOX("CCT Desc is Invalid!");
		return E_FAIL;
	}

	m_pController = m_pManager->createController(capsuleDesc);
	if (!m_pController)
	{
		delete m_pHitReport;
		return E_FAIL;
	}
	m_pController->getActor()->userData = m_pOwner;
	Set_Position(m_pOwnerTransform->Get_WorldPos());

#ifdef _DEBUG
	CGameInstance::GetInstance()->Get_CollisionSystem()->RegisterCCT(this);
#endif

	m_fHeight = pDesc->fHeight;
	m_fRadius = pDesc->fRadius;

	return S_OK;
}

void CCharacterController::OnCollisionEnter(ICollidable* pOther)
{
	m_CurrentCollisions.insert(pOther);
	m_pOwner->OnCollisionEnter();
}

void CCharacterController::OnCollisionStay(ICollidable* pOther)
{
	m_pOwner->OnCollisionStay();
}

void CCharacterController::OnCollisionExit(ICollidable* pOther)
{
	m_CurrentCollisions.erase(pOther);
	m_pOwner->OnCollisionExit();
}


void CCharacterController::Update(_float dt)
{
	if (!m_pController) return;
	Apply_Gravity(dt);
	Apply_Move(dt);
}

void CCharacterController::Late_Update(_float dt)
{
	if (!m_pController) return;
	// PhysX -> Transform
	const PxExtendedVec3& position = m_pController->getPosition();
	m_pOwnerTransform->Set_WorldPos(XMVectorSet((float)position.x, (float)position.y, (float)position.z, 1.f));
}

void CCharacterController::Render_GUI()
{
	if (!Get_CompActive()) return;

	ImGui::SeparatorText("CharacterController");
	if (ImGui::BeginChild("##CCTInfo", ImVec2(0, 200), true))
	{
		ImGui::Text("Grounded: %s", m_bGrounded ? "True" : "False");

		if (m_pController)
		{
			PxExtendedVec3 pos = m_pController->getPosition();
			ImGui::Text("Position: (%.2f, %.2f, %.2f)", (float)pos.x, (float)pos.y, (float)pos.z);

			PxExtendedVec3 foot = m_pController->getFootPosition();
			ImGui::Text("Foot Pos: (%.2f, %.2f, %.2f)", (float)foot.x, (float)foot.y, (float)foot.z);

			ImGui::Text("Height: %.2f", m_fHeight);
			ImGui::Text("Radius: %.2f", m_fRadius);
		}

		ImGui::Separator();
		ImGui::Text("Colliding: %s", IsColliding() ? "True" : "False");
		ImGui::Text("Collision Count: %d", m_CurrentCollisions.size());

		if (!m_CurrentCollisions.empty())
		{
			ImGui::Separator();
			ImGui::Text("Colliding With:");
			for (auto pOther : m_CurrentCollisions)
			{
				if (pOther && pOther->Get_Owner())
				{
					const char* typeStr = static_cast<CCollider*>(pOther) != nullptr ? "[COL]" : "[CCT]";
					ImGui::BulletText("%s %s", typeStr, pOther->Get_Owner()->Get_InstanceName().c_str());
				}
			}
		}
	}
	ImGui::EndChild();
}

#ifdef _DEBUG
void CCharacterController::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	if (!m_pController) return;

	const PxExtendedVec3& pos = m_pController->getPosition();
	XMFLOAT3 vPos((float)pos.x, (float)pos.y, (float)pos.z);

	BoundingOrientedBox obb;
	obb.Center = vPos;
	obb.Extents = _float3(m_fRadius, m_fHeight * 0.5f + m_fRadius, m_fRadius);
	obb.Orientation = _float4(0.f, 0.f, 0.f, 1.f);

	DX::Draw(pBatch, obb, vColor);
}
#endif

void CCharacterController::Move(_fvector vDisp, _float fMinDist, _float dt)
{
	if (!m_pController) return;

	_float3 vDisplacement;
	XMStoreFloat3(&vDisplacement, vDisp);
	PxVec3 disp(vDisplacement.x, vDisplacement.y, vDisplacement.z);

	PxControllerFilters filters;
	// 이동 실행 (충돌 발생 시 내부적으로 m_pHitReport->on... 호출됨)
	const PxControllerCollisionFlags flags = m_pController->move(disp, fMinDist, dt, filters);

	m_bGrounded = (flags & PxControllerCollisionFlag::eCOLLISION_DOWN);
}

void CCharacterController::Jump(_float fJump)
{
	m_vVelocity.y = fJump;
	m_bGrounded = false;
}

void CCharacterController::Set_Position(_fvector vPos)
{
	if (!m_pController) return;

	_float3 p;
	XMStoreFloat3(&p, vPos);
	m_pController->setPosition(PxExtendedVec3(p.x, p.y, p.z));
	m_pOwnerTransform->Set_WorldPos(vPos);
}

void CCharacterController::Resize(_float fHeight, _float fRadius)
{
	if (!m_pController) return;

	PxCapsuleController* pCapsule = static_cast<PxCapsuleController*>(m_pController);
	if (pCapsule)
	{
		pCapsule->setHeight(fHeight);
		pCapsule->setRadius(fRadius);
	}
}

_vector CCharacterController::Get_FootPosition()
{
	if (!m_pController) return XMVectorZero();

	const PxExtendedVec3& pos = m_pController->getFootPosition();
	return XMVectorSet((float)pos.x, (float)pos.y, (float)pos.z, 1.f);
}

PxShape* CCharacterController::Get_Shape()
{
	if (!m_pController) return nullptr;

	PxRigidDynamic* pActor = m_pController->getActor();
	if (!pActor) return nullptr;

	PxShape* pShape = nullptr;
	pActor->getShapes(&pShape, 1);
	return pShape;
}

void CCharacterController::Apply_Gravity(_float dt)
{
	if (m_bGrounded)
	{
		// 바닥에 있을 때: 약간의 하방 압력 유지 (땅에 붙어있기 위해)
		if (m_vVelocity.y <= 0.f)			// 점프 직후(Y > 0)일 때는 적용하지 않음
			m_vVelocity.y = -1.0f;
	}
	else
	{
		m_vVelocity.y += -9.81f * dt;			// 공중에 있을 때: 중력 가속도 누적
	}
}

void CCharacterController::Apply_Move(_float dt)
{
	_vector vDisplacement = XMLoadFloat3(&m_vVelocity) * dt;
	_float3 vDisp; XMStoreFloat3(&vDisp, vDisplacement);
	PxVec3 pxDisp(vDisp.x, vDisp.y, vDisp.z);
	PxControllerFilters filters;

	// PhysX Move 호출
	const PxControllerCollisionFlags flags = m_pController->move(pxDisp, 0.001f, dt, filters);

	// 바닥 체크 결과 갱신
	m_bGrounded = (flags & PxControllerCollisionFlag::eCOLLISION_DOWN);
}

void CCharacterController::Process_ShapeHit(const PxControllerShapeHit& hit)
{
	// 벽이나 물체
	PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();
	if (actor && !(actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC))
	{
		const PxReal pushForce = 5.0f;
		if (hit.dir.y < 0.1f) // 밟고 있는게 아니라면 (옆으로 밀 때만)
			actor->addForce(hit.dir * pushForce, PxForceMode::eIMPULSE);
	}
}

void CCharacterController::Process_ControllerHit(const PxControllersHit& hit)
{
	// 다른 캐릭터
}

void CCharacterController::Process_ObstacleHit(const PxControllerObstacleHit& hit)
{
	// Obstacle
}

CCharacterController* CCharacterController::Create()
{
	CCharacterController* pInstance = new CCharacterController();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCharacterController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CCharacterController::Clone()
{
	return new CCharacterController(*this);
}

void CCharacterController::Free()
{

#ifdef _DEBUG
	if (m_pPhysicsSystem)
	{
		CGameInstance::GetInstance()->Get_CollisionSystem()->UnregisterCCT(this);
	}
#endif

	if (m_pHitReport)
	{
		delete m_pHitReport;
		m_pHitReport = nullptr;
	}

	if (m_pController)
	{
		m_pController->release();
		m_pController = nullptr;
	}

	__super::Free();
}

