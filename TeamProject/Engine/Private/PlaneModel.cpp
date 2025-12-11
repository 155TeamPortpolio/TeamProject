#include "Engine_Defines.h"
#include "PlaneModel.h"
#include "VI_Rect.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "GameObject.h"
#include "Transform.h"

CPlaneModel::CPlaneModel()
{
}

CPlaneModel::CPlaneModel(const CPlaneModel& rhs)
	:CModel(rhs), m_pPlane{ rhs.m_pPlane }
{
	Safe_AddRef(m_pPlane);
}

HRESULT CPlaneModel::Initialize_Prototype()
{
	m_pPlane = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(G_GlobalLevelKey, "Engine_Default_Plane", BUFFER_TYPE::BASIC_PLANE);
	Safe_AddRef(m_pPlane);

	return S_OK;
}

HRESULT CPlaneModel::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

CPlaneModel* CPlaneModel::Create()
{
	CPlaneModel* instance = new CPlaneModel();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CPlaneModel Create Failed : CPlaneModel");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CPlaneModel::Clone()
{
	CPlaneModel* instance = new CPlaneModel(*this);
	return instance;
}


const D3D11_INPUT_ELEMENT_DESC* CPlaneModel::Get_ElementDesc(_uint DrawIndex)
{
	return VTXNORMTEX::Elements;
}

const _uint CPlaneModel::Get_ElementCount(_uint DrawIndex)
{
	return VTXNORMTEX::iElementCount;
}

const string_view CPlaneModel::Get_ElementKey(_uint DrawIndex)
{
	return VTXNORMTEX::Key;
}

HRESULT CPlaneModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
	m_pPlane->Bind_Buffer(pContext);
	m_pPlane->Render(pContext);
	return S_OK;
}

HRESULT CPlaneModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
	Safe_Release(m_pPlane);
	m_pPlane = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(levelKey, modelDataKey, BUFFER_TYPE::BASIC_PLANE);
	Safe_AddRef(m_pPlane);

	return S_OK;
}

_uint CPlaneModel::Get_MeshCount()
{
	return 1;
}

_uint CPlaneModel::Get_MaterialIndex(_uint Index)
{
	return 0;
}

_bool CPlaneModel::isDrawable(_uint Index)
{
	return isDrawing;
}

void CPlaneModel::SetDrawable(_uint Index, _bool isDraw)
{
	isDrawing = isDraw;
}

MINMAX_BOX CPlaneModel::Get_LocalBoundingBox()
{
	return MINMAX_BOX{ { -0.5f, 0.f, -0.5f, }, {0.5f,0.f ,0.5f} };
}

MINMAX_BOX CPlaneModel::Get_WorldBoundingBox()
{
	MINMAX_BOX wordlBox = {};
	_float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
	XMStoreFloat3(&wordlBox.vMin, XMVector3TransformCoord({ -0.5f, 0.f, -0.5f }, XMLoadFloat4x4(pWorldMat)));
	XMStoreFloat3(&wordlBox.vMax, XMVector3TransformCoord({ 0.5f,0.f ,0.5f }, XMLoadFloat4x4(pWorldMat)));
	return wordlBox;
}

vector<MINMAX_BOX> CPlaneModel::Get_MeshBoundingBox()
{
	vector<MINMAX_BOX> boxes;
	boxes.push_back(MINMAX_BOX{ { -0.5f, 0.f, -0.5f, }, {0.5f,0.f ,0.5f} });
	return boxes;
}

MINMAX_BOX CPlaneModel::Get_MeshBoundingBox(_uint index)
{
	return MINMAX_BOX{ { -0.5f, 0.f, -0.5f, }, {0.5f,0.f ,0.5f} };
}

void CPlaneModel::Render_GUI()
{
}

void CPlaneModel::Free()
{
	__super::Free();
	Safe_Release(m_pPlane);
}
