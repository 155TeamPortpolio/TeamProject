#pragma once
#include "Engine_Defines.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "DebugDraw.h"
#include "StaticModel.h"
#include "SkeletalModel.h"

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

void CCharacterController::Set_VerticalVelocity(_float fVelocity)
{
	m_vVelocity.y = fVelocity;
}

void CCharacterController::Set_MaxSpeed(_float fMaxSpeed)
{
	m_fMaxSpeed = fMaxSpeed;
}

void CCharacterController::Set_GravityEnabled(_bool bEnabled)
{
	m_bGravityEnabled = bEnabled;
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

	CCT_DESC* pDesc = {nullptr};
	if (pArg)
	{
		pDesc = static_cast<CCT_DESC*>(pArg);
		if (pDesc->bAutoFit)
		{
			AutoFit(pDesc);
		}
	}

	m_pMaterial = m_pPhysicsSystem->Get_Material(pDesc->strMaterialTag);

	PxUserControllerHitReport* pHitReport =
		CGameInstance::GetInstance()->Get_CollisionSystem()->Get_CCTCallback();

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
		_vector3 vPos = m_pOwnerTransform->Get_WorldPos();
		capsuleDesc.position = PxExtendedVec3(vPos.x, vPos.y, vPos.z);
	}
	else
	{
		capsuleDesc.position = PxExtendedVec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z);
	}


	capsuleDesc.reportCallback = pHitReport;
	capsuleDesc.behaviorCallback = nullptr;

	if (!capsuleDesc.isValid())
	{
		MSG_BOX("CCT Desc is Invalid!");
		return E_FAIL;
	}

	m_pController = m_pManager->createController(capsuleDesc);
	if (!m_pController)
	{
		MSG_BOX("Failed to Create Controller");
		return E_FAIL;
	}

	m_pController->getActor()->userData = m_pOwner;
	Set_Position(m_pOwnerTransform->Get_WorldPos());

	PxShape* pShape;
	m_pController->getActor()->getShapes(&pShape, 1);
	pShape->userData = this;

	PxFilterData filterData;
	filterData.word0 = 1 << ENUM(pDesc->eGroup);
	filterData.word1 = pDesc->iCollisionMask;
	pShape->setSimulationFilterData(filterData); // 시뮬레이션용 필터
	pShape->setQueryFilterData(filterData);      // 레이캐스팅용 필터
	m_FilterData = filterData;

	CGameInstance::GetInstance()->Get_CollisionSystem()->RegisterCollidable(this, -1);

	m_fHeight = pDesc->fHeight;
	m_fRadius = pDesc->fRadius;
	m_fStepOffset = pDesc->fStepOffset;
	m_fSlopeLimit = pDesc->fSlopeLimit;
	m_fMaxSpeed = pDesc->fMaxSpeed;

	return S_OK;
}

void CCharacterController::OnCollisionEnter(ICollidable* pOther)
{
	m_pOwner->OnCollisionEnter();
}

void CCharacterController::OnCollisionStay(ICollidable* pOther)
{
	m_pOwner->OnCollisionStay();
}

void CCharacterController::OnCollisionExit(ICollidable* pOther)
{
	m_pOwner->OnCollisionExit();
}

void CCharacterController::OnTriggerEnter(ICollidable* pOther)
{
	m_pOwner->OnTriggerEnter();
}

void CCharacterController::OnTriggerExit(ICollidable* pOthter)
{
	m_pOwner->OnTriggerExit();
}


void CCharacterController::Update(_float dt)
{
	if (!m_pController) return;
	Apply_Gravity(dt);
}

void CCharacterController::Late_Update(_float dt)
{
	if (!m_pController) return;
	Apply_Move(dt);
	// PhysX -> Transform
	const PxExtendedVec3& position = m_pController->getPosition();
	m_pOwnerTransform->Set_WorldPos(XMVectorSet((float)position.x, (float)position.y, (float)position.z, 1.f));
}

void CCharacterController::Render_GUI()
{
	if (!Get_CompActive()) return;

	ImGui::SeparatorText("CharacterController");

	if (ImGui::BeginChild("##CCTInfo", ImVec2(0, 400), true))
	{
		ImGui::Text("Grounded: %s", m_bGrounded ? "True" : "False");

		if (m_pController)
		{
			PxExtendedVec3 pos = m_pController->getPosition();
			_float fPos[3] = { (_float)pos.x, (_float)pos.y, (_float)pos.z };
			if (ImGui::DragFloat3("Position", fPos, 0.1f))
			{
				Set_Position(_vector4(fPos[0], fPos[1], fPos[2], 1.f));
			}

			PxExtendedVec3 foot = m_pController->getFootPosition();
			ImGui::Text("Foot Pos: (%.2f, %.2f, %.2f)", (float)foot.x, (float)foot.y, (float)foot.z);

			ImGui::Separator();
			ImGui::Text("Properties");

			_bool bGravity = m_bGravityEnabled;
			if (ImGui::Checkbox("Gravity Enabled", &bGravity))
			{
				Set_GravityEnabled(bGravity);
			}

			_float fGravity = m_fGravity;
			if (ImGui::DragFloat("Gravity", &fGravity, 0.01f, -100.f, 0.0f))
			{
				Set_Gravity(fGravity);
			}

			_float fHeight = m_fHeight;
			if (ImGui::DragFloat("Height", &fHeight, 0.01f, 0.1f, 10.0f))
			{
				Resize(fHeight, m_fRadius);
			}

			_float fRadius = m_fRadius;
			if (ImGui::DragFloat("Radius", &fRadius, 0.01f, 0.1f, 5.0f))
			{
				Resize(m_fHeight, fRadius);
			}

			_float fStepOffset = m_fStepOffset;
			if (ImGui::DragFloat("Step Offset", &fStepOffset, 0.01f, 0.0f, 2.0f))
			{
				Set_StepOffset(fStepOffset);
			}

			_float fSlopeLimit = m_fSlopeLimit;
			if (ImGui::DragFloat("Slope Limit", &fSlopeLimit, 0.1f, 0.0f, 90.0f))
			{
				Set_SlopeLimit(fSlopeLimit);
			}

			ImGui::Separator();
			ImGui::Text("Velocity");
			ImGui::Text("X: %.2f | Y: %.2f | Z: %.2f", m_vVelocity.x, m_vVelocity.y, m_vVelocity.z);

			_float fSpeed = sqrtf(m_vVelocity.x * m_vVelocity.x + m_vVelocity.z * m_vVelocity.z);
			ImGui::Text("Planar Speed: %.2f m/s", fSpeed);

			_float fMaxSpeed = m_fMaxSpeed;
			if (ImGui::DragFloat("Max Speed", &fMaxSpeed, 0.1f, 0.0f, 100.0f))
			{
				Set_MaxSpeed(fMaxSpeed);
			}

			if (ImGui::Button("Reset Velocity"))
			{
				m_vVelocity = _float3(0.f, 0.f, 0.f);
			}
		}

		ImGui::Separator();
		ImGui::Text("Colliding: %s", IsColliding() ? "True" : "False");
		ImGui::Text("Collision Count: %d", m_CurrentCollisions.size());

		if (!m_CurrentCollisions.empty())
		{
			ImGui::Separator();
			ImGui::Text("Colliding With:");

			// 안전한 순회를 위해 벡터에 복사
			vector<ICollidable*> collisionSnapshot;
			collisionSnapshot.reserve(m_CurrentCollisions.size());
			for (auto pOther : m_CurrentCollisions)
			{
				if (pOther)
					collisionSnapshot.push_back(pOther);
			}

			for (auto pOther : collisionSnapshot)
			{
				// 추가 안전성 체크
				if (!pOther || !pOther->Get_Owner()) continue;

				const char* typeStr = "[???]";
				CCollider* pCollider = dynamic_cast<CCollider*>(pOther);
				CCharacterController* pCCT = dynamic_cast<CCharacterController*>(pOther);

				if (pCollider)
					typeStr = "[COL]";
				else if (pCCT)
					typeStr = "[CCT]";

				ImGui::BulletText("%s %s", typeStr, pOther->Get_Owner()->Get_InstanceName().c_str());
			}
		}

#ifdef _DEBUG
		ImGui::Separator();
		ImGui::Text("Debug Ray");
		ImGui::Checkbox("Show Ray", &m_bShowDebugRay);

		if (m_bShowDebugRay)
		{
			ImGui::Text("Ray Start: (%.2f, %.2f, %.2f)",
				m_vRayStart.x, m_vRayStart.y, m_vRayStart.z);
			ImGui::Text("Ray End: (%.2f, %.2f, %.2f)",
				m_vRayEnd.x, m_vRayEnd.y, m_vRayEnd.z);

			if (m_DebugRayHit.bHit)
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Hit!");
				ImGui::Text("Distance: %.2f", m_DebugRayHit.fDistance);
				ImGui::Text("Hit Point: (%.2f, %.2f, %.2f)",
					m_DebugRayHit.vPoint.x,
					m_DebugRayHit.vPoint.y,
					m_DebugRayHit.vPoint.z);
				ImGui::Text("Normal: (%.2f, %.2f, %.2f)",
					m_DebugRayHit.vNormal.x,
					m_DebugRayHit.vNormal.y,
					m_DebugRayHit.vNormal.z);

				if (m_DebugRayHit.pHitObject)
				{
					ImGui::Text("Hit Object: %s",
						m_DebugRayHit.pHitObject->Get_InstanceName().c_str());
				}
			}
			else
			{
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "No Hit");
			}

			if (ImGui::Button("Clear Ray"))
			{
				Clear_DebugRay();
			}
		}
#endif
	}
	ImGui::EndChild();
}

void CCharacterController::Process_Response(const PxControllerShapeHit& hit)
{
	PxRigidDynamic* pDynamic = hit.actor->is<PxRigidDynamic>();
	if (pDynamic && !(pDynamic->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC))
	{
		const PxReal pushForce = 5.0f;
		if (hit.dir.y < 0.1f)
			pDynamic->addForce(hit.dir * pushForce, PxForceMode::eIMPULSE);
	}
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

	// 레이 시각화
	if (m_bShowDebugRay)
	{
		Render_DebugRay(pBatch);
	}
}
void CCharacterController::Render_DebugRay(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	_vector vStart = XMLoadFloat3(&m_vRayStart);
	_vector vEnd = XMLoadFloat3(&m_vRayEnd);

	// 충돌 여부에 따라 색상 결정
	XMVECTOR vLineColor = m_DebugRayHit.bHit ? Colors::Red : Colors::Yellow;

	// 레이 라인 그리기
	pBatch->DrawLine(
		VertexPositionColor(vStart, vLineColor),
		VertexPositionColor(vEnd, vLineColor)
	);

	// 충돌 지점 표시
	if (m_DebugRayHit.bHit)
	{
		// 충돌 지점에 작은 구체 그리기
		BoundingSphere hitSphere;
		hitSphere.Center = m_DebugRayHit.vPoint;
		hitSphere.Radius = 0.1f;
		DX::Draw(pBatch, hitSphere, Colors::Red);

		// 법선 벡터 표시 (선택사항)
		_vector vHitPoint = XMLoadFloat3(&m_DebugRayHit.vPoint);
		_vector vNormal = XMLoadFloat3(&m_DebugRayHit.vNormal);
		_vector vNormalEnd = vHitPoint + vNormal * 0.5f;

		pBatch->DrawLine(
			VertexPositionColor(vHitPoint, Colors::Blue),
			VertexPositionColor(vNormalEnd, Colors::Cyan)
		);
	}
}
#endif

void CCharacterController::Move_Direction(_fvector vDir, _float fSpeed)
{
	if (!m_pController) return;

	_vector vNormalized = XMVector3Normalize(vDir);
	Set_PlanarVelocity(vNormalized * fSpeed);
}

void CCharacterController::Move_Velocity(_fvector vVelocity)
{
	if (!m_pController) return;

	_float3 vVel;
	XMStoreFloat3(&vVel, vVelocity);
	m_vVelocity.x = vVel.x;
	m_vVelocity.z = vVel.z;
}

void CCharacterController::Stop_Movement()
{
	m_vVelocity.x = 0.f;
	m_vVelocity.z = 0.f;
}

void CCharacterController::Move(_fvector vDisp, _float dt)
{
	if (!m_pController) return;

	_float3 vDisplacement;
	XMStoreFloat3(&vDisplacement, vDisp);
	PxVec3 disp(vDisplacement.x, vDisplacement.y, vDisplacement.z);

	PxControllerFilters filters;
	filters.mFilterData = &m_FilterData;

	const PxControllerCollisionFlags flags = m_pController->move(disp, 0.001f, dt, filters);
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

	_vector3 p = vPos;
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
		m_fHeight = fHeight;
		m_fRadius = fRadius;
	}
}

void CCharacterController::Set_StepOffset(_float fOffset)
{
	if (!m_pController) return;

	m_fStepOffset = fOffset;
	m_pController->setStepOffset(fOffset);
}

void CCharacterController::Set_SlopeLimit(_float fDegree)
{
	if (!m_pController) return;

	m_fSlopeLimit = fDegree;
	m_pController->setSlopeLimit(cosf(XMConvertToRadians(fDegree)));
}

_vector CCharacterController::Get_FootPosition()
{
	if (!m_pController) return XMVectorZero();

	const PxExtendedVec3& pos = m_pController->getFootPosition();
	return XMVectorSet((float)pos.x, (float)pos.y, (float)pos.z, 1.f);
}

_bool CCharacterController::Shoot_Ray(_fvector vDirection, _float fDistance)
{
	if (!m_pController) return false;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	IPhysicsService* pPhysics = pGameInstance->Get_PhysicsSystem();

	const PxExtendedVec3& pos = m_pController->getPosition();
	_vector vOrigin = XMVectorSet((float)pos.x, (float)pos.y, (float)pos.z, 1.f);
	_vector vDir = XMVector3Normalize(vDirection);

	vOrigin += vDir * (m_fRadius + 0.1f);

	PHYSICS_RAY rayDesc;
	XMStoreFloat3(&rayDesc.vOrigin, vOrigin);
	XMStoreFloat3(&rayDesc.vDirection, vDir);
	rayDesc.fMaxDistance = fDistance;
	rayDesc.iCollisionMask = 0xFFFFFFFF;
	rayDesc.bQueryTrigger = false;

#ifdef _DEBUG
	m_bShowDebugRay = true;
	m_vRayStart = rayDesc.vOrigin;

	_vector vEndPos = vOrigin + vDir * fDistance;
	XMStoreFloat3(&m_vRayEnd, vEndPos);
#endif

	PHYSICS_RAY_HIT hit;
	_bool bResult = pPhysics->Raycast(rayDesc, hit);

#ifdef _DEBUG
	if (bResult)
	{
		m_DebugRayHit = hit;
		XMStoreFloat3(&m_vRayEnd, XMLoadFloat3(&hit.vPoint));
	}
	else
	{
		// 히트하지 않았을 때 - 이전 정보 초기화
		m_DebugRayHit.bHit = false;
		m_DebugRayHit.fDistance = 0.f;
		m_DebugRayHit.pHitObject = nullptr;
		m_DebugRayHit.pCollidable = nullptr;
		m_DebugRayHit.pShape = nullptr;
		// vRayEnd는 최대 거리 위치로 유지
	}
#endif

	return bResult;
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
	if (!m_bGravityEnabled) return;
	if (m_bGrounded)
	{
		// 바닥에 있을 때: 약간의 하방 압력 유지 (땅에 붙어있기 위해)
		if (m_vVelocity.y <= 0.f)			// 점프 직후(Y > 0)일 때는 적용하지 않음
			m_vVelocity.y = -1.0f;
	}
	else
	{
		m_vVelocity.y += m_fGravity * dt;			// 공중에 있을 때: 중력 가속도 누적
	}
}

void CCharacterController::Apply_Move(_float dt)
{
	_vector3 vVelocity = m_vVelocity;

	if (m_fMaxSpeed > 0.0f)
	{
		_float fPlanarSpeed = sqrtf(vVelocity.x * vVelocity.x + vVelocity.z * vVelocity.z);
		if (fPlanarSpeed > m_fMaxSpeed)
		{
			_float fScale = m_fMaxSpeed / fPlanarSpeed;
			vVelocity.x *= fScale;
			vVelocity.z *= fScale;
		}
	}

	_vector3 vDisplacement = vVelocity * dt;
	PxVec3 pxDisp(vDisplacement.x, vDisplacement.y, vDisplacement.z);

	PxControllerFilters filters;
	filters.mFilterData = &m_FilterData;

	const PxControllerCollisionFlags flags = m_pController->move(pxDisp, 0.001f, dt, filters);
	m_bGrounded = (flags & PxControllerCollisionFlag::eCOLLISION_DOWN);
}

HRESULT CCharacterController::AutoFit(CCT_DESC* pDesc)
{
	CStaticModel* pStaticModel = m_pOwner->Get_Component<CStaticModel>();
	CSkeletalModel* pSkeletalModel = m_pOwner->Get_Component<CSkeletalModel>();

	MINMAX_BOX boundingBox = {};
	_bool bHasModel = false;

	if (pStaticModel)
	{
		boundingBox = pStaticModel->Get_LocalBoundingBox();
		bHasModel = true;
	}
	else if (pSkeletalModel)
	{
		boundingBox = pSkeletalModel->Get_LocalBoundingBox();
		bHasModel = true;
	}

	if (!bHasModel)
	{
		return E_FAIL;
	}

	_float3 vMin = boundingBox.vMin;
	_float3 vMax = boundingBox.vMax;

	// Radius 계산: XZ 평면에서의 최대 반지름
	_float fRadiusX = (vMax.x - vMin.x) * 0.5f;
	_float fRadiusZ = (vMax.z - vMin.z) * 0.5f;
	_float fRadius = max(fRadiusX, fRadiusZ);

	// Height 계산: Y축 높이에서 위아래 반구 부분 제외
	_float fTotalHeight = vMax.y - vMin.y;
	_float fCylinderHeight = fTotalHeight - (fRadius * 2.0f);

	// 최소값 보장
	if (fCylinderHeight < 0.1f)
	{
		fCylinderHeight = 0.1f;
	}

	// 스케일 적용
	pDesc->fRadius = fRadius * pDesc->fRadiusScale * pDesc->fSizeScale;
	pDesc->fHeight = fCylinderHeight * pDesc->fHeightScale * pDesc->fSizeScale;

	// 초기 위치가 설정되지 않았다면 바운딩 박스 중심으로 설정
	if (pDesc->vPos.x == 0.f && pDesc->vPos.y == 0.f && pDesc->vPos.z == 0.f)
	{
		pDesc->vPos.x = (vMin.x + vMax.x) * 0.5f;
		pDesc->vPos.y = vMin.y + pDesc->fRadius;  // 발 위치를 바닥에 맞춤
		pDesc->vPos.z = (vMin.z + vMax.z) * 0.5f;
	}

	// StepOffset 자동 조정
	pDesc->fStepOffset = pDesc->fHeight * 0.25f;  // 높이의 25%를 계단 오를 수 있는 높이로 설정

	return S_OK;
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
	CGameInstance::GetInstance()->Get_CollisionSystem()->UnRegisterCollidable(this, -1);

	if (m_pController)
	{
		m_pController->release();
		m_pController = nullptr;
	}

	__super::Free();
}

