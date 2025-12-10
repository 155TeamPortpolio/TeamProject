#include "RayReceiver.h"
#include "GameInstance.h"
#include "RaySystem.h"
#include "GameObject.h"
#include "Model.h"
#include "Transform.h"

CRayReceiver::CRayReceiver()
{
}

CRayReceiver::CRayReceiver(const CRayReceiver& rhs)
	:CComponent(rhs), m_ReturnToManager(rhs.m_ReturnToManager)
{
}

HRESULT CRayReceiver::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRayReceiver::Initialize(COMPONENT_DESC* pArg)
{
	m_ID = CGameInstance::GetInstance()->Get_RayMgr()->Register_RayReceiver(this);
	if (m_pTransform == nullptr || m_pModel == nullptr) {
		m_pTransform = m_pOwner->Get_Component<CTransform>();
		m_pModel = m_pOwner->Get_Component<CModel>();
	}
	return S_OK;
}

_bool CRayReceiver::OnRayHit(RAY* ray, RAY_HIT* HittedInfo)
{
	if (!m_bActive) return false;
	if (ray == nullptr) return false;

	m_tHitInfo = {};
	m_bIsHit = false;

	Check_VaildComponent();

	if (!m_pTransform || !m_pModel) return false;
	
	_matrix WorldInverse = XMLoadFloat4x4(m_pTransform->Get_InverseWorldMatrix_Ptr());

	_float3 RayLocalDir = {};
	XMStoreFloat3(
		&RayLocalDir,
		XMVector3Normalize(
			XMVector3TransformNormal(
				XMLoadFloat3(&ray->vRayDirection),
				WorldInverse
			)
		)
	);

	_float3 RayLocalOrigin = {};
	XMStoreFloat3(
		&RayLocalOrigin,
		XMVector3TransformCoord(
			XMLoadFloat3(&ray->vRayOrigin),
			WorldInverse
		)
	);

	MINMAX_BOX bounding = m_pModel->Get_LocalBoundingBox();

	_float tmin = -FLT_MAX;
	_float tmax = FLT_MAX;
	for (size_t i = 0; i < 3; i++)
	{
		_float fNowMin = {};
		_float fNowMax = {};

		//축마다 순회 도는 로직으로
		_float DirAxis = reinterpret_cast<_float*>(&RayLocalDir.x)[i];
		_float OriginAxis = reinterpret_cast<_float*>(&RayLocalOrigin.x)[i];
		_float MinAxis = reinterpret_cast<_float*>(&bounding.vMin.x)[i];
		_float MaxAxis = reinterpret_cast<_float*>(&bounding.vMax.x)[i];

		if (fabs(DirAxis) < 1e-6f) //방향이 축과 평행하다면 =>
		{
			//그리고 원래 위치가 최소보다 작거나 최대보다 크면(안겹침)
			if (OriginAxis < MinAxis || MaxAxis < OriginAxis)
				return false;
		}
		else
		{	 //평행하지 않으면
			//tmin하고 tmax 사이에서 교차 판정을 하기로 하는 것. 언제부터 언제까지 쯤에 교차할건지
			// (RayOrigin.x + t * RayDir.x = bounding.vMin.x -이게 최소 x에 교차했다는 뜻)
			fNowMin = (MinAxis - OriginAxis) / DirAxis;
			fNowMax = (MaxAxis - OriginAxis) / DirAxis;
			if (fNowMin > fNowMax) swap(fNowMin, fNowMax);
		}

		tmin = max(fNowMin, tmin);
		tmax = min(fNowMax, tmax);

	}
	if (tmin > tmax) return false; // 교차 구간 없음

	_vector hitLocal = XMLoadFloat3(&RayLocalOrigin) + tmin * XMLoadFloat3(&RayLocalDir);
	_vector hitWorld = XMVector3TransformCoord(hitLocal, XMLoadFloat4x4(m_pTransform->Get_WorldMatrix_Ptr()));
	XMStoreFloat3(&m_tHitInfo.vHittedPosition, hitWorld);

	_vector vDiff = hitWorld - XMLoadFloat3(&ray->vRayOrigin);
	m_tHitInfo.fDistance = XMVectorGetX(XMVector3Length(vDiff));
	m_tHitInfo.pObject = m_pOwner;

	if (m_tHitInfo.fDistance > ray->fMaxDistance) {
		m_tHitInfo = {};
		return false;
	}
	m_bIsHit = true;
	if(!m_ReturnToManager)
		return false;

	*HittedInfo = m_tHitInfo;
	return true;
}

_float3 CRayReceiver::Get_RayHittedPos(_bool* isHit)
{
	*isHit = m_bIsHit;
	return  m_tHitInfo.vHittedPosition; 
}

void CRayReceiver::Check_VaildComponent()
{
	if (m_pTransform == nullptr || m_pModel == nullptr) {
		m_pTransform = m_pOwner->Get_Component<CTransform>();
		m_pModel = m_pOwner->Get_Component<CModel>();
	}
}



CRayReceiver* CRayReceiver::Create()
{
	CRayReceiver* instance = new CRayReceiver();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("CRayReceiver Comp Failed To Create : CRayReceiver");
	}
	return instance;
}

CComponent* CRayReceiver::Clone()
{
	return new CRayReceiver(*this);
}


void CRayReceiver::Free()
{
	__super::Free();
	//CGameInstance::GetInstance()->Get_RayMgr()->UnRegister_RayReceiver(m_ID);
}

/*
	if (fabs(RayLocalDir.x) < 1e-6f)
	{
		if (RayLocalOrigin.x < bounding.vMin.x || RayLocalOrigin.x > bounding.vMax.x)
			return false;
	}
	else
	{
		//tmin하고 tmax 사이에서 교차 판정을 하기로 하는 것.
		tmin = (bounding.vMin.x - RayLocalOrigin.x) / RayLocalDir.x;
		tmax = (bounding.vMax.x - RayLocalOrigin.x) / RayLocalDir.x;
		if (tmin > tmax) swap(tmin, tmax);
	}

	if (RayLocalDir.y < 1e-6f)
		RayLocalDir.y = 1e-6f;
_float tYmin = (bounding.vMin.y - RayLocalOrigin.y) / RayLocalDir.y;
_float tYmax = (bounding.vMax.y - RayLocalOrigin.y) / RayLocalDir.y;
if (tYmin > tYmax) swap(tYmin, tYmax);


if (tYmax < tmin) //y최대가 x최소보다 작다면,
	return false;
if (tYmin > tmax) //x최대가 y최소보다 작다면,
return false;

//교차 하는게 확인되었으니 이번에 더 작은걸로 교체
if (tYmin > tmin) tmin = tYmin;
if (tYmax < tmax) tmax = tYmax;

//똑같이 Z축
if (RayLocalDir.z < 1e-6f)
	RayLocalDir.z = 1e-6f;
_float tZmin = (bounding.vMin.z - RayLocalOrigin.z) / RayLocalDir.z;
_float tZmax = (bounding.vMax.z - RayLocalOrigin.z) / RayLocalDir.z;
if (tZmin > tZmax) swap(tZmin, tZmax);

if (tZmax < tmin) //z최대가 x최소보다 작다면,
	return false;
if (tZmin > tmax) //x최대가 z최소보다 작다면,
return false;

if (tZmin > tmin) tmin = tZmin;
if (tZmax < tmax) tmax = tZmax;

_vector hitLocal = XMLoadFloat4(&RayLocalOrigin) + tmin * XMLoadFloat4(&RayLocalDir);
_vector hitWorld = XMVector3TransformCoord(hitLocal, XMLoadFloat4x4(m_pTransform->Get_WorldMatrix()));
XMStoreFloat3(&m_tHitInfo.vHittedPosition, hitWorld);
m_tHitInfo.fDistance = tmin;
m_tHitInfo.pObject = m_pOwner;

HittedInfo = &m_tHitInfo;
return true;
*/