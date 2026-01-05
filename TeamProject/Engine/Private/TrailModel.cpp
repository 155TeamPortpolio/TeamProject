#include "Engine_Defines.h"
#include "TrailModel.h"
#include "GameObject.h"
#include "Transform.h"

CTrailModel::CTrailModel()
{
}

CTrailModel::CTrailModel(const CTrailModel& rhs)
{
}

HRESULT CTrailModel::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CTrailModel::Initialize(COMPONENT_DESC* pArg)
{
	return E_NOTIMPL;
}

const D3D11_INPUT_ELEMENT_DESC* CTrailModel::Get_ElementDesc(_uint DrawIndex)
{
	return nullptr;
}

const _uint CTrailModel::Get_ElementCount(_uint DrawIndex)
{
	return _uint();
}

const string_view CTrailModel::Get_ElementKey(_uint DrawIndex)
{
	return string_view();
}

HRESULT CTrailModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
	return E_NOTIMPL;
}

HRESULT CTrailModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
	return E_NOTIMPL;
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
}
