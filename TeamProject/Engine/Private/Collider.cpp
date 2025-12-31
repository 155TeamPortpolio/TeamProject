#include "Engine_Defines.h"
#include "Collider.h"
#include "GameInstance.h"
#include "ICollisionService.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "CharacterController.h"
#include "StaticModel.h"
#include "SkeletalModel.h"

#include "DebugDraw.h"


CCollider::CCollider()
{
}

CCollider::CCollider(const CCollider& rhs)
	:ICollidable(rhs)
	, m_pShape(nullptr)
	, m_pAttachedRigidBody(nullptr)
	, m_pStaticActor(nullptr)
{
}

HRESULT CCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider::Initialize(COMPONENT_DESC* pArg)
{
	m_pPhysicsSystem = CGameInstance::GetInstance()->Get_PhysicsSystem();
	if (!m_pPhysicsSystem) return E_FAIL;

	m_pOwnerTransform = m_pOwner->Get_Component<CTransform>();
	if (!m_pOwnerTransform) return E_FAIL;

	m_pAttachedRigidBody = m_pOwner->Get_Component<CRigidBody>();

	if (!pArg) return S_OK;
	COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);

	if (pDesc->bAutoFit && !pDesc->bCooking)
	{
		AutoFit(pDesc);
	}

	PxGeometry* pGeometry = nullptr;
	// 쿠킹이 필요한 경우
	if (pDesc->bCooking)
	{
		m_bCooked = true;

		if (m_pAttachedRigidBody)
		{
			MSG_BOX("CCollider::Initialize : Cooked Mesh cannot have RigidBody");
			return E_FAIL;
		}

		CStaticModel* pModel = m_pOwner->Get_Component<CStaticModel>();
		if (!pModel)
		{
			MSG_BOX("CCollider::Initialize : No StaticModel for Cooking");
			return E_FAIL;
		}

		m_pTriangleMesh = m_pPhysicsSystem->Cook_TriangleMesh(pDesc->strModelKey, pModel);

		if (!m_pTriangleMesh)
		{
			MSG_BOX("CCollider::Initialize : Failed to Cook TriangleMesh");
			return E_FAIL;
		}

		pGeometry = new PxTriangleMeshGeometry(m_pTriangleMesh);
	}
	else
	{
		switch (pDesc->eType)
		{
		case COLLIDER_TYPE::BOX:
			pGeometry = new PxBoxGeometry(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f);
			break;
		case COLLIDER_TYPE::SPHERE:
			pGeometry = new PxSphereGeometry(pDesc->vSize.x);
			break;
		case COLLIDER_TYPE::CAPSULE:
			pGeometry = new PxCapsuleGeometry(pDesc->vSize.x, pDesc->vSize.y * 0.5f);
			break;
		}
	}

	if (!pGeometry)
	{
		MSG_BOX("CCollider::Initialize : Failed to Create Geometry");
		return E_FAIL;
	}

	if (m_pAttachedRigidBody)	// Dynamic Actor(RigidBody O)
	{
		m_pShape = m_pAttachedRigidBody->Attach_Shape(*pGeometry, pDesc->strMaterialTag);
	}
	else						// Static Actor(RigidBody X)
	{
		PxPhysics* pPhysics = m_pPhysicsSystem->Get_Physics();
		PxScene* pScene = m_pPhysicsSystem->Get_Scene();
		if (!pPhysics || !pScene)
		{
			delete pGeometry;
			return E_FAIL;
		}

		_vector vPos = m_pOwnerTransform->Get_WorldPos();
		_matrix mWorldMat = XMLoadFloat4x4(m_pOwnerTransform->Get_WorldMatrix_Ptr());
		_vector vScale, vRot, vTrans;
		XMMatrixDecompose(&vScale, &vRot, &vTrans, mWorldMat);

		PxTransform initialPose(
			PxVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos)),
			PxQuat(XMVectorGetX(vRot), XMVectorGetY(vRot), XMVectorGetZ(vRot), XMVectorGetW(vRot))
		);

		m_pStaticActor = pPhysics->createRigidStatic(initialPose);
		if (!m_pStaticActor)
		{
			delete pGeometry;
			return E_FAIL;
		}

		PxMaterial* pMaterial = m_pPhysicsSystem->Get_Material(pDesc->strMaterialTag);
		m_pShape = PxRigidActorExt::createExclusiveShape(*m_pStaticActor, *pGeometry, *pMaterial);

		m_pStaticActor->userData = m_pOwner;
		pScene->addActor(*m_pStaticActor);
	}

	if (!m_pShape)
	{
		delete pGeometry;
		MSG_BOX("CCollider::Initialize : Failed to Create Shape");
		return E_FAIL;
	}

	// Shape Flag
	m_pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	m_pShape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	m_pShape->setContactOffset(0.02f);  // 기본값: 0.02
	m_pShape->setRestOffset(0.0f);      // 관통 허용 거리 최소화
	if (pDesc->bTrigger) {
		m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}

	PxFilterData filterData;
	filterData.word0 = 1 << ENUM(pDesc->eGroup);
	filterData.word1 = pDesc->iCollisionMask;
	m_pShape->setSimulationFilterData(filterData); // 시뮬레이션용 필터
	m_pShape->setQueryFilterData(filterData);      // 레이캐스팅용 필터

	if (!m_bCooked)
	{
		_vector3 vPos = pDesc->vCenter;
		_vector4 vRot = XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);

		PxTransform localPose;
		localPose.p = PxVec3(vPos.x, vPos.y, vPos.z);
		localPose.q = PxQuat(vRot.x, vRot.y, vRot.z, vRot.w);
		m_pShape->setLocalPose(localPose);
	}

	m_pShape->userData = this;		// UserData 설정
	delete pGeometry;				// Geometry 메모리 해제

	// 멤버 변수 저장
	m_eType = pDesc->eType;
	m_eGroup = pDesc->eGroup;
	m_iCollisionMask = pDesc->iCollisionMask;
	m_vCenter = pDesc->vCenter;
	m_vSize = pDesc->vSize;
	m_vRotation = pDesc->vRotation;
	m_bTrigger = pDesc->bTrigger;
	m_strMaterialTag = pDesc->strMaterialTag;

	// 시스템 등록
	CGameInstance::GetInstance()->Get_CollisionSystem()->RegisterCollidable(this, -1);

	return S_OK;
}

void CCollider::Update(_float dt)
{
	if (m_bMapTool)
	{
		Sync_Transform();
	}
}

void CCollider::OnCollisionEnter(ICollidable* pOther)
{
	m_pOwner->OnCollisionEnter();
	m_pOwner->OnCollisionEnter(pOther->Get_Owner());
}

void CCollider::OnCollisionStay(ICollidable* pOther)
{
	m_pOwner->OnCollisionStay();
	m_pOwner->OnCollisionStay(pOther->Get_Owner());
}

void CCollider::OnCollisionExit(ICollidable* pOther)
{
	m_pOwner->OnCollisionExit();
	m_pOwner->OnCollisionExit(pOther->Get_Owner());
}

void CCollider::OnTriggerEnter(ICollidable* pOther)
{
	m_pOwner->OnTriggerEnter();
	m_pOwner->OnTriggerEnter(pOther->Get_Owner());
}

void CCollider::OnTriggerExit(ICollidable* pOther)
{
	m_pOwner->OnTriggerExit();
	m_pOwner->OnTriggerExit(pOther->Get_Owner());
}

void CCollider::Set_Center(const _float3& vCenter)
{
	m_vCenter = vCenter;
	Update_LocalPose();
}

void CCollider::Set_Size(const _float3& vSize)	// 사용자재
{
	if (!m_pShape) return;

	m_vSize = vSize;
	PxGeometry* pGeometry = nullptr;

	switch (m_eType)
	{
	case COLLIDER_TYPE::BOX:
		pGeometry = new PxBoxGeometry(vSize.x * 0.5f, vSize.y * 0.5f, vSize.z * 0.5f);
		break;
	case COLLIDER_TYPE::SPHERE:
		pGeometry = new PxSphereGeometry(vSize.x);
		break;
	case COLLIDER_TYPE::CAPSULE:
		pGeometry = new PxCapsuleGeometry(vSize.x, vSize.y * 0.5f);
		break;
	}

	if (pGeometry)
	{
		m_pShape->setGeometry(*pGeometry);
		delete pGeometry;

		if (m_pAttachedRigidBody && !m_pAttachedRigidBody->Get_Body()->is<PxRigidStatic>())
		{
			PxRigidDynamic* pDynamic = m_pAttachedRigidBody->Get_Body()->is<PxRigidDynamic>();
			if (pDynamic)
			{
				PxRigidBodyExt::updateMassAndInertia(*pDynamic, 1.0f);
			}
		}
	}
}

void CCollider::Set_Rotation(const _float3& vRotation)
{
	m_vRotation = vRotation;
	Update_LocalPose();
}

void CCollider::Set_Trigger(_bool bTrigger)
{
	if (!m_pShape) return;

	m_bTrigger = bTrigger;

	if (m_bTrigger)
	{
		m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	else
	{
		m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
		m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	}
}

void CCollider::Set_ContactOffset(_float fOffset)
{
	if (m_pShape)
	{
		m_pShape->setContactOffset(fOffset);
	}
}

void CCollider::Set_RestOffset(_float fOffset)
{
	if (m_pShape)
	{
		m_pShape->setRestOffset(fOffset);
	}
}

void CCollider::Update_LocalPose()
{
	if (!m_pShape) return;

	_vector3 vPos = m_vCenter;
	_vector4 vRot = XMQuaternionRotationRollPitchYaw(m_vRotation.x, m_vRotation.y, m_vRotation.z);

	_float3 vP = vPos;
	_float4 vQ = vRot;
	PxTransform localPose;
	localPose.p = PxVec3(vP.x, vP.y, vP.z);
	localPose.q = PxQuat(vQ.x, vQ.y, vQ.z, vQ.w);

	m_pShape->setLocalPose(localPose);
}

HRESULT CCollider::AutoFit(COLLIDER_DESC* pDesc)
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

	// Center 계산
	pDesc->vCenter.x = (vMin.x + vMax.x) * 0.5f;
	pDesc->vCenter.y = (vMin.y + vMax.y) * 0.5f;
	pDesc->vCenter.z = (vMin.z + vMax.z) * 0.5f;

	// Size 계산
	switch (pDesc->eType)
	{
	case COLLIDER_TYPE::BOX:
		pDesc->vSize.x = (vMax.x - vMin.x) * pDesc->fSizeScale;
		pDesc->vSize.y = (vMax.y - vMin.y) * pDesc->fSizeScale;
		pDesc->vSize.z = (vMax.z - vMin.z) * pDesc->fSizeScale;
		break;

	case COLLIDER_TYPE::SPHERE:
	{
		_float fRadiusX = (vMax.x - vMin.x) * 0.5f;
		_float fRadiusY = (vMax.y - vMin.y) * 0.5f;
		_float fRadiusZ = (vMax.z - vMin.z) * 0.5f;
		pDesc->vSize.x = max(max(fRadiusX, fRadiusY), fRadiusZ) * pDesc->fSizeScale;
		break;
	}

	case COLLIDER_TYPE::CAPSULE:
	{
		_float fRadiusXZ = max((vMax.x - vMin.x) * 0.5f, (vMax.z - vMin.z) * 0.5f);
		_float fHeight = vMax.y - vMin.y;
		pDesc->vSize.x = fRadiusXZ * pDesc->fSizeScale;
		pDesc->vSize.y = (fHeight - fRadiusXZ * 2.0f) * pDesc->fSizeScale;
		break;
	}
	}

	return S_OK;
}

void CCollider::Sync_Transform()
{
	if (!m_pStaticActor || m_pAttachedRigidBody)
		return;

	_smatrix mWorldMat = m_pOwnerTransform->Get_WorldMatrix();
	_vector vScale, vRot, vTrans;
	XMMatrixDecompose(&vScale, &vRot, &vTrans, mWorldMat);

	PxTransform pose(
		PxVec3(XMVectorGetX(vTrans), XMVectorGetY(vTrans), XMVectorGetZ(vTrans)),
		PxQuat(XMVectorGetX(vRot), XMVectorGetY(vRot),
			XMVectorGetZ(vRot), XMVectorGetW(vRot))
	);
	m_pStaticActor->setGlobalPose(pose);

	if (m_bCooked)
		return;

	PxShape* shape = nullptr;
	if (m_pStaticActor->getShapes(&shape, 1) > 0)
	{
		PxGeometryHolder geomHolder = shape->getGeometry();

		switch (geomHolder.getType())
		{
		case PxGeometryType::eBOX:
			geomHolder.box().halfExtents = PxVec3(
				m_vSize.x * XMVectorGetX(vScale) * 0.5f,
				m_vSize.y * XMVectorGetY(vScale) * 0.5f,
				m_vSize.z * XMVectorGetZ(vScale) * 0.5f
			);
			break;
		case PxGeometryType::eSPHERE:
			geomHolder.sphere().radius = m_vSize.x * XMVectorGetX(vScale);
			break;
		case PxGeometryType::eCAPSULE:
			geomHolder.capsule().radius = m_vSize.x * XMVectorGetX(vScale);
			geomHolder.capsule().halfHeight = m_vSize.y * XMVectorGetY(vScale) * 0.5f;
			break;
		default:
			return;
		}

		shape->setGeometry(geomHolder.any());
	}
}

void CCollider::Render_GUI()
{
	if (!Get_CompActive()) return;

	ImGui::SeparatorText("Collider");

	if (ImGui::BeginChild("##ColliderChild", ImVec2(0, 350), true))
	{
		ImGui::Text("Type: %s", m_eType == COLLIDER_TYPE::BOX ? "Box" :
			m_eType == COLLIDER_TYPE::SPHERE ? "Sphere" : "Capsule");

		ImGui::Text("Mode: %s", m_pAttachedRigidBody ? "Dynamic" : "Static");

		_bool bTrigger = m_bTrigger;
		if (ImGui::Checkbox("Is Trigger", &bTrigger))
		{
			Set_Trigger(bTrigger);
		}

		if (m_pStaticActor && !m_pAttachedRigidBody)
		{
			ImGui::Checkbox("MapTool Mode", &m_bMapTool);
		}

		ImGui::Checkbox("Is Render", &m_bDebugRender);
		ImGui::Text("Material: %s", m_strMaterialTag.c_str());

		ImGui::Separator();
		ImGui::Text("Collision Layer: %d", ENUM(m_eGroup));
		ImGui::Text("Collision Mask: %d", m_iCollisionMask);

		// For MapTool
		if (m_pStaticActor && !m_pAttachedRigidBody)
		{
			ImGui::Separator();

			_vector3 vWorldPos = m_pOwnerTransform->Get_WorldPos();
			_vector3 vScale = m_pOwnerTransform->Get_Scale();
			_vector4 vRot = m_pOwnerTransform->Get_QuaternionRotate();

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "World Position");
			_bool bPosChanged = ImGui::DragFloat3("##WorldPos", &vWorldPos.x, 0.1f);

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Scale");
			_bool bScaleChanged = ImGui::DragFloat3("##Scale", &vScale.x, 0.01f);

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Rotation");
			_bool bRotChanged = ImGui::DragFloat4("##Rotation", &vRot.x, 0.01f);

			if (bPosChanged || bScaleChanged || bRotChanged)
			{
				if (bPosChanged) m_pOwnerTransform->Set_Pos(vWorldPos);
				if (bScaleChanged) m_pOwnerTransform->Scale_Vector(vScale);
				if (bRotChanged) m_pOwnerTransform->Set_Quaternion(vRot);
				Sync_Transform();
			}
		}

		ImGui::Separator();
		ImGui::Text("Transform");

		_float3 vCenter = m_vCenter;
		if (ImGui::DragFloat3("Center", &vCenter.x, 0.01f))
		{
			Set_Center(vCenter);
		}

		_float3 vSize = m_vSize;
		if (ImGui::DragFloat3("Size", &vSize.x, 0.01f, 0.01f, 100.0f))
		{
			Set_Size(vSize);
		}

		_float3 vRotation = m_vRotation;
		if (ImGui::DragFloat3("Rotation", &vRotation.x, 0.01f))
		{
			Set_Rotation(vRotation);
		}

		ImGui::Separator();
		ImGui::Text("Advanced");

		_float fContactOffset = m_pShape ? m_pShape->getContactOffset() : 0.02f;
		if (ImGui::DragFloat("Contact Offset", &fContactOffset, 0.001f, 0.0f, 1.0f))
		{
			Set_ContactOffset(fContactOffset);
		}

		_float fRestOffset = m_pShape ? m_pShape->getRestOffset() : 0.0f;
		if (ImGui::DragFloat("Rest Offset", &fRestOffset, 0.001f, -1.0f, 1.0f))
		{
			Set_RestOffset(fRestOffset);
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
	}
	ImGui::EndChild();
}

void CCollider::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	if (!m_pShape || !m_bDebugRender) return;

	PxTransform trans;
	if (m_pAttachedRigidBody)
	{
		trans = PxShapeExt::getGlobalPose(*m_pShape, *m_pAttachedRigidBody->Get_Body());
	}
	else if (m_pStaticActor)
	{
		trans = PxShapeExt::getGlobalPose(*m_pShape, *m_pStaticActor);
	}
	else
	{
		return;
	}

	XMFLOAT3 vPos(trans.p.x, trans.p.y, trans.p.z);
	XMFLOAT4 vRot(trans.q.x, trans.q.y, trans.q.z, trans.q.w);

	if (m_bCooked)
	{
		//// TriangleMesh는 AABB로 렌더링
		//PxBounds3 bounds = m_pTriangleMesh->getLocalBounds();

		//BoundingOrientedBox obb;
		//obb.Center = vPos;
		//obb.Extents = _float3(
		//	(bounds.maximum.x - bounds.minimum.x) * 0.5f,
		//	(bounds.maximum.y - bounds.minimum.y) * 0.5f,
		//	(bounds.maximum.z - bounds.minimum.z) * 0.5f
		//);
		//obb.Orientation = vRot;
		//DX::Draw(pBatch, obb, vColor);
	}
	else if (m_eType == COLLIDER_TYPE::BOX)
	{
		BoundingOrientedBox obb;
		obb.Center = vPos;
		obb.Extents = _float3(m_vSize.x * 0.5f, m_vSize.y * 0.5f, m_vSize.z * 0.5f);
		obb.Orientation = vRot;
		DX::Draw(pBatch, obb, vColor);
	}
	else if (m_eType == COLLIDER_TYPE::SPHERE)
	{
		BoundingSphere sphere;
		sphere.Center = vPos;
		sphere.Radius = m_vSize.x;
		DX::Draw(pBatch, sphere, vColor);
	}
	else if (m_eType == COLLIDER_TYPE::CAPSULE)
	{
		BoundingOrientedBox obb;
		obb.Center = vPos;
		obb.Extents = _float3(m_vSize.x, m_vSize.y * 0.5f + m_vSize.x, m_vSize.x);
		obb.Orientation = vRot;
		DX::Draw(pBatch, obb, vColor);
	}
}

CCollider* CCollider::Create()
{
	CCollider* instance = new CCollider();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("Collider Create Failed : CCollider");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CCollider::Clone()
{
	return new CCollider(*this);
}

void CCollider::Free()
{
	CGameInstance::GetInstance()->Get_CollisionSystem()->UnRegisterCollidable(this, -1);

	if (m_pStaticActor)
	{
		if (m_pStaticActor->getScene())
		{
			m_pStaticActor->getScene()->removeActor(*m_pStaticActor);
		}
		m_pStaticActor->release();
		m_pStaticActor = nullptr;
	}

	m_pShape = nullptr;
	__super::Free();
}