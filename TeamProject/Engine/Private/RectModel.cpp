#include "Engine_Defines.h"
#include "RectModel.h"
#include "VI_Rect.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "GameObject.h"
#include "Transform.h"

CRectModel::CRectModel()
{
}

CRectModel::CRectModel(const CRectModel& rhs)
	:CModel(rhs), m_pRect{ rhs.m_pRect }
{
	Safe_AddRef(m_pRect);
}

HRESULT CRectModel::Initialize_Prototype()
{
	m_pRect = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(G_GlobalLevelKey, "Engine_Default_Rect", BUFFER_TYPE::BASIC_RECT);
	Safe_AddRef(m_pRect);

	return S_OK;
}

HRESULT CRectModel::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

CRectModel* CRectModel::Create()
{
	CRectModel* instance = new CRectModel();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CRectModel Create Failed : CRectModel");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CRectModel::Clone()
{
	CRectModel* instance = new CRectModel(*this);
	return instance;
}


const D3D11_INPUT_ELEMENT_DESC* CRectModel::Get_ElementDesc(_uint DrawIndex)
{
	return VTXNORMTEX::Elements;
}

const _uint CRectModel::Get_ElementCount(_uint DrawIndex)
{
	return VTXNORMTEX::iElementCount;
}

const string_view CRectModel::Get_ElementKey(_uint DrawIndex)
{
	return VTXNORMTEX::Key;
}

HRESULT CRectModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
	m_pRect->Bind_Buffer(pContext);
	m_pRect->Render(pContext);
	return S_OK;
}

HRESULT CRectModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
	Safe_Release(m_pRect);
	m_pRect = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(levelKey, modelDataKey, BUFFER_TYPE::BASIC_RECT);
	Safe_AddRef(m_pRect);

	return S_OK;
}

_uint CRectModel::Get_MeshCount()
{
	return 1;
}

_uint CRectModel::Get_MaterialIndex(_uint Index)
{
	return 0;
}

_bool CRectModel::isDrawable(_uint Index)
{
	return isDrawing;
}

void CRectModel::SetDrawable(_uint Index, _bool isDraw)
{
	isDrawing = isDraw;
}

MINMAX_BOX CRectModel::Get_LocalBoundingBox()
{
	return MINMAX_BOX{ { -0.5f, 0.f, -0.5f, }, {0.5f,0.f ,0.5f} };
}

MINMAX_BOX CRectModel::Get_WorldBoundingBox()
{
	MINMAX_BOX wordlBox = {};
	_float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
	XMStoreFloat3(&wordlBox.vMin, XMVector3TransformCoord({ -0.5f, 0.f, -0.5f }, XMLoadFloat4x4(pWorldMat)));
	XMStoreFloat3(&wordlBox.vMax, XMVector3TransformCoord({ 0.5f,0.f ,0.5f }, XMLoadFloat4x4(pWorldMat)));
	return wordlBox;
}

vector<MINMAX_BOX> CRectModel::Get_MeshBoundingBoxes()
{
	vector<MINMAX_BOX> boxes;
	boxes.push_back(MINMAX_BOX{ { -0.5f, -0.5f, 0.f }, {0.5f,0.5f ,0.f} });
	return boxes;
}

MINMAX_BOX CRectModel::Get_MeshBoundingBox(_uint index)
{
	return MINMAX_BOX{ { -0.5f, -0.5f, 0.f }, {0.5f,0.5f ,0.f} };
}

void CRectModel::Render_GUI()
{
}

void CRectModel::Free()
{
	__super::Free();
	Safe_Release(m_pRect);
}
