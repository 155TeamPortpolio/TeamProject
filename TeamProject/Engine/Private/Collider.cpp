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
	:CComponent(rhs)
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
		MSG_BOX("CCollider::Initialize : Onwer's RigidBody is nullptr!");
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
	filterData.word0 = 1;       // 나는 1번 그룹이다 (Default Layer)
	filterData.word1 = 1;       // 나는 1번 그룹하고만 충돌한다
	// (만약 바닥도 이 설정을 따르면 서로 word0(1) & word1(1) != 0 이라 충돌함)
	// 만약 pDesc에 레이어 정보가 있다면 그걸 쓰면 더 좋습니다.
	// 지금은 테스트를 위해 강제 설정합니다.
	m_pShape->setSimulationFilterData(filterData); // 시뮬레이션용 필터
	m_pShape->setQueryFilterData(filterData);      // 레이캐스팅용 필터

	_vector vPos = XMLoadFloat3(&pDesc->vCenter);
	_vector vRot = XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);
	PxTransform localPose;
	_float3 vP; XMStoreFloat3(&vP, vPos);
	_float4 vQ; XMStoreFloat4(&vQ, vRot);
	localPose.p = PxVec3(vP.x, vP.y, vP.z);
	localPose.q = PxQuat(vQ.x, vQ.y, vQ.z, vQ.w);
	m_pShape->setLocalPose(localPose);


	m_pShape->userData = this;		// UserData 설정
	delete pGeometry;				// Geometry 메모리 해제

	// 멤버 변수 저장
	m_eType = pDesc->eType;
	m_vCenter = pDesc->vCenter;
	m_vSize = pDesc->vSize;
	m_vRotation = pDesc->vRotation;
	m_bTrigger = pDesc->isTrigger;
	m_strMaterialTag = pDesc->strMaterialTag;

	// 시스템 등록
	CGameInstance::GetInstance()->Get_CollisionSystem()->RegisterCollider(this, -1);

	return S_OK;
}

void CCollider::OnCollisionEnter(CCollider* pOther, const PxContactPair& contactInfo)
{
	m_CurrentCollisions.insert(pOther);
	m_pOwner->OnCollisionEnter();
}

void CCollider::OnCollisionExit(CCollider* pOther)
{
	m_CurrentCollisions.erase(pOther);
	//m_pOwner->OnCollisionExit()
}

void CCollider::OnTriggerEnter(CCollider* pOther)
{

}

void CCollider::OnTriggerExit(CCollider* pOther)
{
}

void CCollider::Render_GUI()
{
	//if(Get_CompActive()){
	//ImGui::SeparatorText("Collider");
	//float childWidth = ImGui::GetContentRegionAvail().x;
	//const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	//const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

	//ImGui::BeginChild("##ColliderChild", ImVec2{ 0, childHeight }, true);
	//ImGui::Text("System Index : %d", m_SystemIndex);
	//string ownerTag = "Owner Tag : " + m_CollisionContext.Owner->Get_Tag();
	//ImGui::Text(ownerTag.c_str());
	//ImGui::Text(m_CollisionContext.EventTag.c_str());
	//ImGui::EndChild();
	//}
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
	CGameInstance::GetInstance()->Get_CollisionSystem()->UnregisterCollider(this, -1);
	m_pShape = nullptr;
	__super::Free();
}



#pragma region OLD

//void CCollider::Set_CompActive(_bool bActive)
//{
//	m_bActive = bActive; 
//	Set_ColliderActive(bActive);
//
//	if(bActive == false)
//		m_CollisionContext.Owner = nullptr;
//}
//
//void CCollider::Releas_Component()
//{
//	int i = 0;
//}

//void CCollider::Set_ColliderActive(_bool Active)
//{
//	if (m_SystemIndex < 0)
//		return;
//
//	else if (Active == false) {
//		CGameInstance::GetInstance()->Get_CollisionSystem()->DeActiveCollider(this, m_SystemIndex);
//	}
//	else if (Active == true && Has_Desc()) {
//		CGameInstance::GetInstance()->Get_CollisionSystem()->ActiveCollider(this, m_SystemIndex);
//	}
//}


//_bool CCollider::Compare_Same(COLLIDER_SLOT* prev, COLLIDER_SLOT* current)
//{
//	if (!prev || !current)
//		return false;
//
//	if (prev->pCollider == nullptr || current->pCollider == nullptr)
//		return false;
//
//	// DEAD 제외
//	if (prev->eState == COLLIDER_SLOT::STATE::DEAD ||
//		current->eState == COLLIDER_SLOT::STATE::DEAD)
//		return false;
//
//	if (prev->iGeneration != current->iGeneration)
//		return false;
//
//	return prev->pCollider == current->pCollider;
//}

#pragma endregion