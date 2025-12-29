#pragma once
#include "Engine_Defines.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "DebugDraw.h"
#include "StaticModel.h"
#include "SkeletalModel.h"

void CCharacterController::Set_Velocity(_fvector vVelocity)
{
	_float3 vIn;
	XMStoreFloat3(&vIn, vVelocity);
	m_vVelocity.x = vIn.x;
	m_vVelocity.y = vIn.y;
	m_vVelocity.z = vIn.z;
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

void CCharacterController::Set_FootPosition(_vector3 vFootPos)
{
	_float fCenterY = vFootPos.y + m_fFootOffset + m_fRadius + (m_fHeight * 0.5f);
	PxExtendedVec3 pos(vFootPos.x, fCenterY, vFootPos.z);
	m_pController->setPosition(pos);
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
		else
		{
			m_fBoundingMinY = pDesc->fBoundingMinY;
		}

		// 음수면 발이 바닥에 묻히는 상태 -> 양수 오프셋으로 보정
		if (m_fBoundingMinY < 0.f)
			m_fFootOffset = -m_fBoundingMinY;
		else
			m_fFootOffset = 0.f;
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
	capsuleDesc.contactOffset = max(pDesc->fContactOffset, 0.01f);
	capsuleDesc.upDirection = PxVec3(0, 1, 0);
	capsuleDesc.density = pDesc->fDensity;
	capsuleDesc.position = PxExtendedVec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z);

	_float fAdjustY = pDesc->vPos.y + m_fFootOffset + pDesc->fRadius + (pDesc->fHeight * 0.5f);
	capsuleDesc.position = PxExtendedVec3(pDesc->vPos.x, fAdjustY, pDesc->vPos.z);

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
	Set_Position(XMLoadFloat3(&pDesc->vPos));

	PxShape* pShape;
	m_pController->getActor()->getShapes(&pShape, 1);
	pShape->userData = this;
	pShape->setContactOffset(pDesc->fContactOffset);
	pShape->setRestOffset(pDesc->fRestOffset);

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

	m_pQueryFilter = new CCCTQueryFilter(&m_FilterData);

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
	// Apply_Move
	_float3 vDisplacement = m_vVelocity * dt;
	if (m_fMaxSpeed > 0.0f)
	{
		_float fPlanarSpeed = sqrtf(vDisplacement.x * vDisplacement.x +
			vDisplacement.z * vDisplacement.z);
		if (fPlanarSpeed > m_fMaxSpeed * dt)
		{
			_float fScale = (m_fMaxSpeed * dt) / fPlanarSpeed;
			vDisplacement.x *= fScale;
			vDisplacement.z *= fScale;
		}
	}
	Move(XMLoadFloat3(&vDisplacement), dt);

	// Update Position
	const PxExtendedVec3& footPosition = m_pController->getFootPosition();
	_float fTransformY = (float)footPosition.y - m_fBoundingMinY;

	m_pOwnerTransform->Set_WorldPos(XMVectorSet(
		(float)footPosition.x,
		fTransformY,
		(float)footPosition.z,
		1.f));
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
			_float fPos[3] = { (_float)pos.x, (_float)pos.y, (_float)pos.z };
			if (ImGui::DragFloat3("Position", fPos, 0.1f))
			{
				Set_Position(_vector4(fPos[0], fPos[1], fPos[2], 1.f));
			}

			PxExtendedVec3 foot = m_pController->getFootPosition();
			ImGui::Text("Foot Pos: (%.2f, %.2f, %.2f)", (float)foot.x, (float)foot.y, (float)foot.z);

			_vector3 vWorldPos = m_pOwnerTransform->Get_WorldPos();
			ImGui::Text("Transform Pos: (%.2f, %.2f, %.2f)", vWorldPos.x, vWorldPos.y, vWorldPos.z);

			_float3 vLocalFoot;
			vLocalFoot.x = (float)foot.x - vWorldPos.x;
			vLocalFoot.y = (float)foot.y - vWorldPos.y;
			vLocalFoot.z = (float)foot.z - vWorldPos.z;
			ImGui::Text("Foot Local: (%.2f, %.2f, %.2f)", vLocalFoot.x, vLocalFoot.y, vLocalFoot.z);

			ImGui::Separator();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Debug Info");
			_float fCalculatedCenter = (float)foot.y + m_fRadius + (m_fHeight * 0.5f);
			ImGui::Text("Calculated Center Y: %.2f", fCalculatedCenter);
			ImGui::Text("Actual Center Y: %.2f", (float)pos.y);
			ImGui::Text("Difference: %.2f", fCalculatedCenter - (float)pos.y);
			ImGui::Text("Height: %.2f, Radius: %.2f", m_fHeight, m_fRadius);

			ImGui::Separator();
			ImGui::TextColored(ImVec4(0, 1, 1, 1), "Foot Offset Adjustment");
			_float fBoundingMinY = m_fBoundingMinY;
			if (ImGui::DragFloat("Bounding Min Y", &fBoundingMinY, 0.01f, -10.0f, 10.0f))
			{
				Set_BoundingMinY(fBoundingMinY);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Local Foot Offset Adjustment");
			}

			if (ImGui::Button("Reset Foot Offset"))
			{
				Set_BoundingMinY(0.0f);
			}
			ImGui::SameLine();
			if (ImGui::Button("Re-Apply AutoFit"))
			{
				CCT_DESC desc;
				desc.fRadiusScale = 1.0f;
				desc.fHeightScale = 1.0f;
				desc.fSizeScale = 1.0f;
				AutoFit(&desc);
			}

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

			vector<ICollidable*> collisionSnapshot;
			collisionSnapshot.reserve(m_CurrentCollisions.size());
			for (auto pOther : m_CurrentCollisions)
			{
				if (pOther)
					collisionSnapshot.push_back(pOther);
			}

			for (auto pOther : collisionSnapshot)
			{
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

		ImGui::Separator();
		ImGui::Text("Collision Offsets");

		_float fContactOffset = Get_ContactOffset();
		if (ImGui::DragFloat("Contact Offset", &fContactOffset, 0.0001f, 0.0001f, 0.1f, "%.4f"))
		{
			Set_ContactOffset(fContactOffset);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Collision Detect Distance");
		}

		_float fRestOffset = Get_RestOffset();
		if (ImGui::DragFloat("Rest Offset", &fRestOffset, 0.0001f, -0.01f, 0.01f, "%.4f"))
		{
			Set_RestOffset(fRestOffset);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Contact Offset");
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
		if (hit.dir.y < 0.0001f)
			pDynamic->addForce(hit.dir * pushForce, PxForceMode::eIMPULSE);
	}
}

void CCharacterController::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
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

void CCharacterController::Move_Direction(_fvector vDir, _float fSpeed, _float dt)
{
	_vector3 vDirection = vDir;
	vDirection.Normalize();
	vDirection = vDirection * fSpeed * dt;
	vDirection.y = m_vVelocity.y * dt;
	Move(vDirection, dt);
}

void CCharacterController::Move_Velocity(_fvector vVelocity, _float dt)
{
	_vector3 vVel = vVelocity;
	vVel.y = m_vVelocity.y;
	Move(vVel * dt, dt);
}

void CCharacterController::Move_Displacement(_fvector vDisp, _float dt)
{
	_vector3 vDisplacement = vDisp;
	vDisplacement.y += m_vVelocity.y * dt;
	Move(vDisplacement, dt);
}

void CCharacterController::Move_RootMotion(_fvector vLocalDelta, _fvector qRotation, _float dt)
{
	const _float fRootMotionScale = 1.f;

	_vector3 vDelta = vLocalDelta;
	_vector3 vLocalMotion = _vector3(vDelta.x, 0.f, -vDelta.z);

	_smatrix matRot = _smatrix::CreateFromQuaternion(qRotation);
	_vector3 vWorldMotion = _vector3::Transform(vLocalMotion, matRot);

	vWorldMotion *= fRootMotionScale;
	vWorldMotion.y = m_vVelocity.y * dt;

	Move(vWorldMotion, 1.f);
}

void CCharacterController::Stop_Movement()
{
	m_vVelocity = {};
}

void CCharacterController::Set_CollisionMask(_uint iMask)
{
	m_FilterData.word1 = iMask;

	PxShape* pShape;
	m_pController->getActor()->getShapes(&pShape, 1);
	if (pShape)
	{
		pShape->setSimulationFilterData(m_FilterData);
		pShape->setQueryFilterData(m_FilterData);
	}
}

void CCharacterController::Set_CollisionGroup(COLLISION_GROUP eGroup)
{
	m_FilterData.word0 = 1 << ENUM(eGroup);

	PxShape* pShape;
	m_pController->getActor()->getShapes(&pShape, 1);
	if (pShape)
	{
		pShape->setSimulationFilterData(m_FilterData);
		pShape->setQueryFilterData(m_FilterData);
	}
}

void CCharacterController::Move(_fvector vDisplacement, _float dt)
{
	_float3 vDisp;
	XMStoreFloat3(&vDisp, vDisplacement);
	PxVec3 pxDisp(vDisp.x, vDisp.y, vDisp.z);

	// Scene Query 1차 필터용: 마스크를 word0에 설정
	m_QueryFilterData.word0 = m_FilterData.word1;

	PxControllerFilters filters;
	filters.mFilterData = &m_QueryFilterData;
	filters.mFilterCallback = m_pQueryFilter;
	filters.mFilterFlags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;

	const PxControllerCollisionFlags flags = m_pController->move(pxDisp, 0.0001f, dt, filters);
	m_bGrounded = (flags & PxControllerCollisionFlag::eCOLLISION_DOWN);
}

void CCharacterController::Jump(_float fJump)
{
	m_vVelocity.y = fJump;
	m_bGrounded = false;
}

void CCharacterController::Set_Position(_fvector vPos)
{
	_vector3 p = vPos;
	m_pController->setPosition(PxExtendedVec3(p.x, p.y, p.z));
	m_pOwnerTransform->Set_WorldPos(vPos);
}

void CCharacterController::Resize(_float fHeight, _float fRadius)
{
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
	m_fStepOffset = fOffset;
	m_pController->setStepOffset(fOffset);
}

void CCharacterController::Set_SlopeLimit(_float fDegree)
{
	m_fSlopeLimit = fDegree;
	m_pController->setSlopeLimit(cosf(XMConvertToRadians(fDegree)));
}

_vector CCharacterController::Get_FootPosition()
{
	const PxExtendedVec3& pos = m_pController->getFootPosition();
	return XMVectorSet((float)pos.x, (float)pos.y, (float)pos.z, 1.f);
}

_bool CCharacterController::Shoot_Ray(_fvector vDirection, _float fDistance, PHYSICS_RAY_HIT& hit)
{
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
	m_fBoundingMinY = vMin.y;

	_float fRadiusX = (vMax.x - vMin.x) * 0.5f;
	_float fRadiusZ = (vMax.z - vMin.z) * 0.5f;
	_float fRadius = max(fRadiusX, fRadiusZ);

	_float fTotalHeight = vMax.y - vMin.y;
	_float fCylinderHeight = fTotalHeight - (fRadius * 2.0f);

	if (fCylinderHeight < 0.1f)
	{
		fCylinderHeight = 0.1f;
	}

	pDesc->fRadius = fRadius * pDesc->fRadiusScale * pDesc->fSizeScale;
	pDesc->fHeight = fCylinderHeight * pDesc->fHeightScale * pDesc->fSizeScale;

	_vector3 vWorldPos = m_pOwnerTransform->Get_WorldPos();

	_float fFootY = vWorldPos.y + vMin.y;

	pDesc->vPos.x = vWorldPos.x;
	pDesc->vPos.y = fFootY + pDesc->fRadius + (pDesc->fHeight * 0.5f);
	pDesc->vPos.z = vWorldPos.z;

	pDesc->fStepOffset = pDesc->fHeight * 0.25f;

	return S_OK;
}

void CCharacterController::Set_ContactOffset(_float fOffset)
{
	m_fContactOffset = fOffset;
	if (m_pController)
	{
		PxShape* pShape;
		m_pController->getActor()->getShapes(&pShape, 1);
		if (pShape)
		{
			pShape->setContactOffset(fOffset);
		}
	}
}

void CCharacterController::Set_RestOffset(_float fOffset)
{
	m_fRestOffset = fOffset;
	PxShape* pShape;
	m_pController->getActor()->getShapes(&pShape, 1);
	if (pShape)
	{
		pShape->setRestOffset(fOffset);
	}
}

_float CCharacterController::Get_ContactOffset()
{
	PxShape* pShape;
	m_pController->getActor()->getShapes(&pShape, 1);
	if (pShape)
	{
		return pShape->getContactOffset();
	}
	return m_fContactOffset;
}

_float CCharacterController::Get_RestOffset()
{
    PxShape* pShape;
    m_pController->getActor()->getShapes(&pShape, 1);
    if (pShape)
    {
    	return pShape->getRestOffset();
    }
	return m_fRestOffset;
}

void CCharacterController::Set_BoundingMinY(_float fMinY)
{
	_float fDelta = fMinY - m_fBoundingMinY;
	m_fBoundingMinY = fMinY;

	const PxExtendedVec3& currentPos = m_pController->getPosition();
	PxExtendedVec3 newPos = currentPos;
	newPos.y += fDelta;

	m_pController->setPosition(newPos);
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

	if (m_pQueryFilter)
	{
		delete m_pQueryFilter;
		m_pQueryFilter = nullptr;
	}

	if (m_pController)
	{
		m_pController->release();
		m_pController = nullptr;
	}

	__super::Free();
}

