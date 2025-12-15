#include "Engine_Defines.h"
#include "Collider.h"
#include "GameInstance.h"
#include "ICollisionService.h"
#include "GameObject.h"
#include "RigidBody.h"

#ifdef _DEBUG
#include "DebugDraw.h"
#endif

CCollider::CCollider()
{
}

CCollider::CCollider(const CCollider& rhs)
	:ICollidable(rhs)
	,m_pShape(nullptr)
	,m_pAttachedRigidBody(nullptr)
{
}

HRESULT CCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider::Initialize(COMPONENT_DESC* pArg)
{
	if (!pArg) return E_FAIL;
	COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);

	// RigidBody 컴포넌트 찾기 : Collider 이전에 RigidBody가 먼저 추가되어야함
	m_pAttachedRigidBody = m_pOwner->Get_Component<CRigidBody>();
	if (m_pAttachedRigidBody == nullptr)
	{
		MSG_BOX("CCollider::Initialize : Owner must have RigidBody component! Add RigidBody before Collider.");
		return E_FAIL;
	}

	// Geometry(모양) 생성
	PxGeometry* pGeometry = nullptr;
	switch (pDesc->eType)
	{
	case COLLIDER_TYPE::BOX: // Box: HalfExtents(x,y,z)
		pGeometry = new PxBoxGeometry(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f);
		break;
	case COLLIDER_TYPE::SPHERE: // Sphere: Radius(x)
		pGeometry = new PxSphereGeometry(pDesc->vSize.x);
		break;
	case COLLIDER_TYPE::CAPSULE: // Capsule: Radius(x)/HalfHeight(y)
		pGeometry = new PxCapsuleGeometry(pDesc->vSize.x, pDesc->vSize.y * 0.5f);
		break;
	}

	if (!pGeometry)
	{
		MSG_BOX("CCollider::Initialize : Failed to Create Geometry");
		return E_FAIL;
	}

	m_pShape = m_pAttachedRigidBody->Attach_Shape(*pGeometry, pDesc->strMaterialTag);
	if (!m_pShape)
	{
		delete pGeometry;
		MSG_BOX("CCollider::Initialize : Failed to Attach Shape to RigidBody");
		return E_FAIL;
	}

	// Shape Flag
	m_pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	m_pShape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	m_pShape->setContactOffset(0.02f);  // 기본값: 0.02
	m_pShape->setRestOffset(0.0f);      // 관통 허용 거리 최소화
	if (pDesc->isTrigger) {
		m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}

	PxFilterData filterData;
	filterData.word0 = 1 << ENUM(pDesc->eGroup);
	filterData.word1 = pDesc->iCollisionMask;
	m_pShape->setSimulationFilterData(filterData); // 시뮬레이션용 필터
	m_pShape->setQueryFilterData(filterData);      // 레이캐스팅용 필터

	// 초기 위치 및 회전값 설정
	_vector3 vPos = pDesc->vCenter;
	_vector4 vRot = XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);
	_float3 vP = vPos;
	_float4 vQ = vRot;
	PxTransform localPose;
	localPose.p = PxVec3(vP.x, vP.y, vP.z);
	localPose.q = PxQuat(vQ.x, vQ.y, vQ.z, vQ.w);
	m_pShape->setLocalPose(localPose);


	m_pShape->userData = this;		// UserData 설정
	delete pGeometry;				// Geometry 메모리 해제

	// 멤버 변수 저장
	m_eType = pDesc->eType;
	m_eGroup = pDesc->eGroup;
	m_iCollisionMask = pDesc->iCollisionMask;
	m_vCenter = pDesc->vCenter;
	m_vSize = pDesc->vSize;
	m_vRotation = pDesc->vRotation;
	m_bTrigger = pDesc->isTrigger;
	m_strMaterialTag = pDesc->strMaterialTag;

	// 시스템 등록
	CGameInstance::GetInstance()->Get_CollisionSystem()->RegisterCollidable(this, -1);

	return S_OK;
}

void CCollider::OnCollisionEnter(ICollidable* pOther)
{
	m_pOwner->OnCollisionEnter();
	m_CurrentCollisions.insert(pOther);
}

void CCollider::OnCollisionStay(ICollidable* pOther)
{
	m_pOwner->OnCollisionStay();
}

void CCollider::OnCollisionExit(ICollidable* pOther)
{
	m_pOwner->OnCollisionExit();
	m_CurrentCollisions.erase(pOther);
}

void CCollider::OnTriggerEnter(ICollidable* pOther)
{
	m_pOwner->OnTriggerEnter();
}

void CCollider::OnTriggerExit(ICollidable* pOther)
{
	m_pOwner->OnTriggerExit();
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

void CCollider::Render_GUI()
{
	if (!Get_CompActive()) return;

	ImGui::SeparatorText("Collider");

	if (ImGui::BeginChild("##ColliderChild", ImVec2(0, 300), true))
	{
		ImGui::Text("Type: %s", m_eType == COLLIDER_TYPE::BOX ? "Box" :
			m_eType == COLLIDER_TYPE::SPHERE ? "Sphere" : "Capsule");

		_bool bTrigger = m_bTrigger;
		if (ImGui::Checkbox("Is Trigger", &bTrigger))
		{
			Set_Trigger(bTrigger);
		}

		ImGui::Text("Material: %s", m_strMaterialTag.c_str());

		ImGui::Separator();
		ImGui::Text("Collision Layer: %d", ENUM(m_eGroup));
		ImGui::Text("Collision Mask: %d", m_iCollisionMask);

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
			for (auto pOther : m_CurrentCollisions)
			{
				if (pOther && pOther->Get_Owner())
				{
					ImGui::BulletText("%s", pOther->Get_Owner()->Get_InstanceName().c_str());
				}
			}
		}
	}
	ImGui::EndChild();
}

#ifdef _DEBUG
void CCollider::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	if (!m_pShape || !m_pAttachedRigidBody || !m_pAttachedRigidBody->Get_Body()) return;

	PxTransform trans = PxShapeExt::getGlobalPose(*m_pShape, *m_pAttachedRigidBody->Get_Body());

	// DirectX Math로 변환
	XMFLOAT3 vPos(trans.p.x, trans.p.y, trans.p.z);
	XMFLOAT4 vRot(trans.q.x, trans.q.y, trans.q.z, trans.q.w);

	// 타입에 따라 DX::Draw 호출
	if (m_eType == COLLIDER_TYPE::BOX)
	{
		BoundingOrientedBox obb;
		obb.Center = vPos;
		obb.Extents = _float3(m_vSize.x * 0.5f, m_vSize.y * 0.5f, m_vSize.z * 0.5f); // Half Size
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
		// 캡슐 전체 크기 근사 (Radius, Height/2 + Radius, Radius)
		obb.Extents = _float3(m_vSize.x, m_vSize.y * 0.5f + m_vSize.x, m_vSize.x);
		obb.Orientation = vRot;
		DX::Draw(pBatch, obb, vColor);
	}
}
#endif

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
	m_pShape = nullptr;
	__super::Free();
}