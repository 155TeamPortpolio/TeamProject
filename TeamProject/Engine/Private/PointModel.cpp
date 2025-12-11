#include "Engine_Defines.h"
#include "PointModel.h"
#include "VI_Point.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "GameObject.h"
#include "Transform.h"

CPointModel::CPointModel()
{
}

CPointModel::CPointModel(const CPointModel& rhs)
	:CModel(rhs), m_pPoint{ rhs.m_pPoint }
{
	Safe_AddRef(m_pPoint);
}

HRESULT CPointModel::Initialize_Prototype()
{
	m_pPoint = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(G_GlobalLevelKey, "Engine_Default_Point", BUFFER_TYPE::BASIC_POINT);
	Safe_AddRef(m_pPoint);

	return S_OK;
}

HRESULT CPointModel::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

CPointModel* CPointModel::Create()
{
	CPointModel* instance = new CPointModel();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CPointModel Create Failed : CPointModel");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CPointModel::Clone()
{
	CPointModel* instance = new CPointModel(*this);
	return instance;
}


const D3D11_INPUT_ELEMENT_DESC* CPointModel::Get_ElementDesc(_uint DrawIndex)
{
	return VTXPOS::Elements;
}

const _uint CPointModel::Get_ElementCount(_uint DrawIndex)
{
	return VTXPOS::iElementCount;
}

const string_view CPointModel::Get_ElementKey(_uint DrawIndex)
{
	return VTXPOS::Key;
}

HRESULT CPointModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
	m_pPoint->Bind_Buffer(pContext);
	m_pPoint->Render(pContext);
	return S_OK;
}

HRESULT CPointModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
	Safe_Release(m_pPoint);
	m_pPoint = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(levelKey, modelDataKey, BUFFER_TYPE::BASIC_POINT);
	Safe_AddRef(m_pPoint);

	return S_OK;
}

_uint CPointModel::Get_MeshCount()
{
	return 1;
}

_uint CPointModel::Get_MaterialIndex(_uint Index)
{
	return 0;
}

_bool CPointModel::isDrawable(_uint Index)
{
	return isDrawing;
}

void CPointModel::SetDrawable(_uint Index, _bool isDraw)
{
	isDrawing = isDraw;
}

MINMAX_BOX CPointModel::Get_LocalBoundingBox()
{
	return MINMAX_BOX{ { -0.5f, 0.f, -0.5f, }, {0.5f,0.f ,0.5f} };
}

MINMAX_BOX CPointModel::Get_WorldBoundingBox()
{
	MINMAX_BOX wordlBox = {};
	_float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
	XMStoreFloat3(&wordlBox.vMin, XMVector3TransformCoord({ -0.5f, 0.f, -0.5f }, XMLoadFloat4x4(pWorldMat)));
	XMStoreFloat3(&wordlBox.vMax, XMVector3TransformCoord({ 0.5f,0.f ,0.5f }, XMLoadFloat4x4(pWorldMat)));
	return wordlBox;
}

vector<MINMAX_BOX> CPointModel::Get_MeshBoundingBoxes()
{
	vector<MINMAX_BOX> boxes;
	boxes.push_back(MINMAX_BOX{ { -0.5f, -0.5f, 0.f }, {0.5f,0.5f ,0.f} });
	return boxes;
}

MINMAX_BOX CPointModel::Get_MeshBoundingBox(_uint index)
{
	return MINMAX_BOX{ { -0.5f, -0.5f, 0.f }, {0.5f,0.5f ,0.f} };
}

void CPointModel::Render_GUI()
{
}

void CPointModel::Free()
{
	__super::Free();
	Safe_Release(m_pPoint);
}
