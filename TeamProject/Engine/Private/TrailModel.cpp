#include "Engine_Defines.h"
#include "TrailModel.h"
#include "GameObject.h"
#include "Transform.h"
#include "VI_Trail.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "GameInstance.h"
#include "Helper_Func.h"

CTrailModel::CTrailModel()
	:CModel()
{
}

CTrailModel::CTrailModel(const CTrailModel& rhs)
	:CModel(rhs)
{
}

HRESULT CTrailModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrailModel::Initialize(COMPONENT_DESC* pArg)
{
	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();

	m_pBuffer = CVI_Trail::Create(pDevice, "Engine_Default_Trail");
	m_TrailVertices.reserve(g_iMaxNumTrailPoints * 2);

	return S_OK;
}

const D3D11_INPUT_ELEMENT_DESC* CTrailModel::Get_ElementDesc(_uint DrawIndex)
{
	return m_pBuffer->Get_ElementDesc();
}

const _uint CTrailModel::Get_ElementCount(_uint DrawIndex)
{
	return m_pBuffer->Get_ElementCount();
}

const string_view CTrailModel::Get_ElementKey(_uint DrawIndex)
{
	return m_pBuffer->Get_ElementKey();
}

HRESULT CTrailModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
	m_pBuffer->Bind_Buffer(pContext);

	return m_pBuffer->Render(pContext);
}

void CTrailModel::SetTrailParams(TRAIL_NODE trailDesc)
{
	m_eMode = static_cast<POINT_MODE>(trailDesc.iMode);
	m_eTextureMode = static_cast<TEXTURE_MODE>(trailDesc.iTextureMode);
	m_eColorMode = static_cast<COLOR_MODE>(trailDesc.iColorMode);

	m_fMaxLifeTime = trailDesc.fMaxLifeTime;
	m_fMinDistance = trailDesc.fMinDistance;

	m_fStartWidth = trailDesc.fStartWidth;
	m_fEndWidth = trailDesc.fEndWidth;

	m_vUVSpeed = trailDesc.vUVSpeed;
	m_fTile = trailDesc.fTile;

	m_vStartColor = trailDesc.vStartColor;
	m_vEndColor = trailDesc.vEndColor;

	m_vUVOffset = _float2(0.f, 0.f);

	auto pMaterialInstance = m_pOwner->Get_Component<CMaterial>()->Get_MaterialInstance(0);
	pMaterialInstance->Set_Param("ColorMode", { &m_eColorMode,"uint",sizeof(_uint) });
	pMaterialInstance->Set_Param("UVOffset", { &m_vUVOffset,"float2",sizeof(_float2) });

	m_CenterPoints.clear();
	m_SegmentPoints.clear();
}

void CTrailModel::Update_CenterPoint(_float3 position, _float dt)
{
	m_vUVOffset.x += m_vUVSpeed.x * dt;
	m_vUVOffset.y += m_vUVSpeed.y * dt;

	CENTER_POINT newPoint{};
	newPoint.vPosition = position;
	newPoint.fLifeTime = 0.f;

	if (m_CenterPoints.empty())
	{
		newPoint.fDistanceAcc = 0.f;
		m_CenterPoints.push_back(newPoint);
	}
	else
	{
		CENTER_POINT lastPoint = m_CenterPoints.back();
		_float fDistance = (_vector3(lastPoint.vPosition) - _vector3(position)).Length();

		if (fDistance >= m_fMinDistance)
		{
			newPoint.fDistanceAcc = m_CenterPoints.back().fDistanceAcc + fDistance;
			m_CenterPoints.push_back(newPoint);
		}
	}

	for (auto& point : m_CenterPoints)
	{
		point.fLifeTime += dt;
		point.fWidth = Math::Lerp(m_fStartWidth, m_fEndWidth, point.fLifeTime / m_fMaxLifeTime);
	}

	if (m_CenterPoints.size() >= g_iMaxNumTrailPoints)
		m_CenterPoints.pop_front();

	while (!m_CenterPoints.empty() && m_CenterPoints.front().fLifeTime >= m_fMaxLifeTime)
		m_CenterPoints.pop_front();

	vector<CENTER_POINT> dividePoints;

	if (m_CenterPoints.size() >= 2)
	{
		Divide_CenterPoints(dividePoints);
		Build_CenterVertices(dividePoints);
	}

	m_pBuffer->Update_Vertices(m_TrailVertices.data(), dividePoints.size());
}

void CTrailModel::Update_SegmentPoint(_float3 position0, _float3 position1, _float dt)
{
	m_vUVOffset.x += m_vUVSpeed.x * dt;
	m_vUVOffset.y += m_vUVSpeed.y * dt;

	SEGMENT_POINT newPoint{};
	newPoint.vPositionA = position0;
	newPoint.vPositionB = position1;
	newPoint.fLifeTime = 0.f;

	if (m_SegmentPoints.empty())
	{
		newPoint.fDistanceAcc = 0.f;
		m_SegmentPoints.push_back(newPoint);
	}
	else
	{
		SEGMENT_POINT lastPoint = m_SegmentPoints.back();
		_vector3 vCenter = (_vector3(lastPoint.vPositionA) + _vector3(lastPoint.vPositionB)) * 0.5f;
		_vector3 vNewCenter = (_vector3(position0) + _vector3(position1)) * 0.5f;
		_float fDistance = (vCenter - _vector3(vNewCenter)).Length();

		if (fDistance >= m_fMinDistance)
		{
			newPoint.fDistanceAcc = m_SegmentPoints.back().fDistanceAcc + fDistance;
			m_SegmentPoints.push_back(newPoint);
		}
	}

	for (auto& point : m_SegmentPoints)
		point.fLifeTime += dt;

	if (m_SegmentPoints.size() >= g_iMaxNumTrailPoints)
		m_SegmentPoints.pop_front();

	while (!m_SegmentPoints.empty() && m_SegmentPoints.front().fLifeTime >= m_fMaxLifeTime)
		m_SegmentPoints.pop_front();

	vector<SEGMENT_POINT> dividePoints;
	
	if (m_SegmentPoints.size() >= 2)
	{
		Divide_SegmentPoints(dividePoints);
		Build_SegmentVertices(dividePoints);
	}

	m_pBuffer->Update_Vertices(m_TrailVertices.data(), dividePoints.size());
}

void CTrailModel::Divide_CenterPoints(vector<CENTER_POINT>& dividePoints)
{
	_uint iPointCount = m_CenterPoints.size();

	dividePoints.reserve(iPointCount + 3 * (iPointCount - 1));

	for (_uint i = 0; i < iPointCount - 1; ++i)
	{
		dividePoints.push_back(m_CenterPoints[i]);

		_float fLife0 = m_CenterPoints[i].fLifeTime;
		_float fLife1 = m_CenterPoints[i + 1].fLifeTime;

		_vector3 p1 = m_CenterPoints[i].vPosition;
		_vector3 p2 = m_CenterPoints[i + 1].vPosition;
		_vector3 p0 = (i == 0) ? p1 : m_CenterPoints[i - 1].vPosition;
		_vector3 p3 = (i + 2 >= iPointCount) ? p2 : m_CenterPoints[i + 2].vPosition;

		_float fLength = (p2 - p1).Length();

		for (_uint j = 0; j < 3; ++j)
		{
			_float t = static_cast<_float>(j + 1) / 4;

			CENTER_POINT divPoint{};
			divPoint.vPosition = _vector3::CatmullRom(p0, p1, p2, p3, t);
			divPoint.fDistanceAcc = 0.f;
			divPoint.fLifeTime = Math::Lerp(fLife0, fLife1, t);
			divPoint.fWidth = Math::Lerp(m_fStartWidth, m_fEndWidth, clamp(divPoint.fLifeTime / m_fMaxLifeTime, 0.f, 1.f));

			dividePoints.push_back(divPoint);
		}
	}

	dividePoints.push_back(m_CenterPoints.back());

	_float fDistanceAcc{};
	for (_uint i = 0; i < dividePoints.size(); ++i)
	{
		if (i > 0)
			fDistanceAcc += (_vector3(dividePoints[i].vPosition) - _vector3(dividePoints[i - 1].vPosition)).Length();

		dividePoints[i].fDistanceAcc = fDistanceAcc;
	}

}

void CTrailModel::Divide_SegmentPoints(vector<SEGMENT_POINT>& dividePoints)
{
	_uint iPointCount = m_SegmentPoints.size();

	dividePoints.reserve(iPointCount + 3 * (iPointCount - 1));

	for (_uint i = 0; i < iPointCount - 1; ++i)
	{
		dividePoints.push_back(m_SegmentPoints[i]);

		_float fLife0 = m_SegmentPoints[i].fLifeTime;
		_float fLife1 = m_SegmentPoints[i + 1].fLifeTime;

		_vector3 p1 = m_SegmentPoints[i].vPositionA;
		_vector3 p2 = m_SegmentPoints[i + 1].vPositionA;
		_vector3 p0 = (i == 0) ? p1 : m_SegmentPoints[i - 1].vPositionA;
		_vector3 p3 = (i + 2 >= iPointCount) ? p2 : m_SegmentPoints[i + 2].vPositionA;

		_vector3 q1 = m_SegmentPoints[i].vPositionB;
		_vector3 q2 = m_SegmentPoints[i + 1].vPositionB;
		_vector3 q0 = (i == 0) ? q1 : m_SegmentPoints[i - 1].vPositionB;
		_vector3 q3 = (i + 2 >= iPointCount) ? q2 : m_SegmentPoints[i + 2].vPositionB;

		_vector3 vCenterA = (_vector3(m_SegmentPoints[i].vPositionA) + _vector3(m_SegmentPoints[i].vPositionB)) * 0.5f;
		_vector3 vCenterB = (_vector3(m_SegmentPoints[i + 1].vPositionA) + _vector3(m_SegmentPoints[i + 1].vPositionB)) * 0.5f;

		_float fLength = (vCenterB - vCenterA).Length();

		for (_uint j = 0; j < 3; ++j)
		{
			_float t = static_cast<_float>(j + 1) / 4;

			SEGMENT_POINT divPoint{};
			divPoint.vPositionA = _vector3::CatmullRom(p0, p1, p2, p3, t);
			divPoint.vPositionB = _vector3::CatmullRom(q0, q1, q2, q3, t);
			divPoint.fDistanceAcc = 0.f;
			divPoint.fLifeTime = Math::Lerp(fLife0, fLife1, t);

			dividePoints.push_back(divPoint);
		}
	}

	dividePoints.push_back(m_SegmentPoints.back());

	_float fDistanceAcc{};
	for (_uint i = 0; i < dividePoints.size(); ++i)
	{
		if (i > 0)
		{
			_vector3 vCenterA = (_vector3(dividePoints[i - 1].vPositionA) + _vector3(dividePoints[i - 1].vPositionB)) * 0.5f;
			_vector3 vCenterB = (_vector3(dividePoints[i].vPositionA) + _vector3(dividePoints[i].vPositionB)) * 0.5f;

			fDistanceAcc += (vCenterB - vCenterA).Length();
		}

		dividePoints[i].fDistanceAcc = fDistanceAcc;
	}
}

void CTrailModel::Build_CenterVertices(vector<CENTER_POINT>& points)
{
	m_TrailVertices.clear();
	
	_uint iPointCount = points.size();
	if (iPointCount < 2) return;

	_vector3 vCamPosition = _vector3(CGameInstance::GetInstance()->Get_CameraMgr()->Get_CameraPos());
	_vector3 vWorldUp(0.f, 1.f, 0.f);
	for (_uint i = 0; i < iPointCount; ++i)
	{
		_vector3 vPosition = points[i].vPosition;
		_vector3 vDir{};
		_vector3 vViewLook = vCamPosition - vPosition;

		if (0 == i)
			vDir = _vector3(points[1].vPosition) - _vector3(points[0].vPosition);
		else if (iPointCount - 1 == i)
			vDir = _vector3(points[i].vPosition) - _vector3(points[i - 1].vPosition);
		else
			vDir = _vector3(points[i + 1].vPosition) - _vector3(points[i - 1].vPosition);

		vDir.Normalize();
		vViewLook.Normalize();

		_vector3 vRight = vDir.Cross(vViewLook);

		/* dir == look */
		if (vRight.Dot(vRight) < 1e-6f)
		{
			vRight = vWorldUp.Cross(vDir);

			if (vRight.Dot(vRight) < 1e-6f)
				vRight = _vector3(1.f, 0.f, 0.f);
		}

		vRight.Normalize();

		VTXTRAIL p0{}, p1{};
		p0.vPosition = vPosition + vRight * points[i].fWidth * 0.5f;
		p0.vLifeTime.x = points[i].fLifeTime;
		p0.vLifeTime.y = m_fMaxLifeTime;
		p0.vColor = _vector4::Lerp(m_vStartColor, m_vEndColor, p0.vLifeTime.x / p0.vLifeTime.y);

		p1.vPosition = vPosition - vRight * points[i].fWidth * 0.5f;
		p1.vLifeTime.x = points[i].fLifeTime;
		p1.vLifeTime.y = m_fMaxLifeTime;
		p1.vColor = _vector4::Lerp(m_vStartColor, m_vEndColor, p1.vLifeTime.x / p1.vLifeTime.y);

		if (TEXTURE_MODE::STRETCH == m_eTextureMode)
		{
			p0.vTexcoord.x = static_cast<_float>(i) / (iPointCount - 1);
			p0.vTexcoord.y = 0.f;

			p1.vTexcoord.x = static_cast<_float>(i) / (iPointCount - 1);
			p1.vTexcoord.y = 1.f;
		}
		else
		{
			p0.vTexcoord.x = points[i].fDistanceAcc * m_fTile;
			p0.vTexcoord.y = 0.f;

			p1.vTexcoord.x = points[i].fDistanceAcc * m_fTile;
			p1.vTexcoord.y = 1.f;
		}

		m_TrailVertices.push_back(p0);
		m_TrailVertices.push_back(p1);
	}
}

void CTrailModel::Build_SegmentVertices(vector<SEGMENT_POINT>& points)
{
	m_TrailVertices.clear();

	_uint iPointCount = points.size();
	if (iPointCount < 2) return;

	for (_uint i = 0; i < iPointCount; ++i)
	{
		SEGMENT_POINT point = points[i];

		VTXTRAIL p0{}, p1{};
		p0.vPosition = point.vPositionA;
		p0.vLifeTime.x = point.fLifeTime;
		p0.vLifeTime.y = m_fMaxLifeTime;
		p0.vColor = _vector4::Lerp(m_vStartColor, m_vEndColor, p0.vLifeTime.x / p0.vLifeTime.y);

		p1.vPosition = point.vPositionB;
		p1.vLifeTime.x = point.fLifeTime;
		p1.vLifeTime.y = m_fMaxLifeTime;
		p1.vColor = _vector4::Lerp(m_vStartColor, m_vEndColor, p1.vLifeTime.x / p1.vLifeTime.y);

		if (TEXTURE_MODE::STRETCH == m_eTextureMode)
		{
			p0.vTexcoord.x = static_cast<_float>(i) / (iPointCount - 1);
			p0.vTexcoord.y = 0.f;

			p1.vTexcoord.x = static_cast<_float>(i) / (iPointCount - 1);
			p1.vTexcoord.y = 1.f;
		}
		else
		{
			p0.vTexcoord.x = point.fDistanceAcc * m_fTile;
			p0.vTexcoord.y = 0.f;

			p1.vTexcoord.x = point.fDistanceAcc * m_fTile;
			p1.vTexcoord.y = 1.f;
		}

		m_TrailVertices.push_back(p0);
		m_TrailVertices.push_back(p1);
	}
}

HRESULT CTrailModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
	return S_OK;
}

_uint CTrailModel::Get_MeshCount()
{
	return 1;
}

_uint CTrailModel::Get_MaterialIndex(_uint Index)
{
	return 0;
}

_bool CTrailModel::isDrawable(_uint Index)
{
	return true;
}

void CTrailModel::SetDrawable(_uint Index, _bool isDraw)
{
}

MINMAX_BOX CTrailModel::Get_LocalBoundingBox()
{
	return MINMAX_BOX{ { -0.5f, 0.f, -0.5f, }, {0.5f,0.f ,0.5f} };
}

MINMAX_BOX CTrailModel::Get_WorldBoundingBox()
{
	MINMAX_BOX wordlBox = {};
	_float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
	XMStoreFloat3(&wordlBox.vMin, XMVector3TransformCoord({ -0.5f, 0.f, -0.5f }, XMLoadFloat4x4(pWorldMat)));
	XMStoreFloat3(&wordlBox.vMax, XMVector3TransformCoord({ 0.5f,0.f ,0.5f }, XMLoadFloat4x4(pWorldMat)));
	return wordlBox;
}

vector<MINMAX_BOX> CTrailModel::Get_MeshBoundingBoxes()
{
	vector<MINMAX_BOX> boxes;
	boxes.push_back(MINMAX_BOX{ { -0.5f, -0.5f, 0.f }, {0.5f,0.5f ,0.f} });
	return boxes;
}

MINMAX_BOX CTrailModel::Get_MeshBoundingBox(_uint index)
{
	return MINMAX_BOX{ { -0.5f, -0.5f, 0.f }, {0.5f,0.5f ,0.f} };
}

CTrailModel* CTrailModel::Create()
{
	CTrailModel* instance = new CTrailModel();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CPointModel Create Failed : CTrailModel");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CTrailModel::Clone()
{
	CTrailModel* instance = new CTrailModel(*this);
	return instance;
}
void CTrailModel::Free()
{
	__super::Free();

	Safe_Release(m_pBuffer);
}
