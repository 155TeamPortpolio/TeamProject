#include "Engine_Defines.h"
#include "TriangleModel.h"
#include "VI_Triangle.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "GameObject.h"
#include "Transform.h"

CTriangleModel::CTriangleModel()
{
}

CTriangleModel::CTriangleModel(const CTriangleModel& rhs)
    :CModel(rhs), m_pTriangle{ rhs.m_pTriangle }
{
    Safe_AddRef(m_pTriangle);
}

HRESULT CTriangleModel::Initialize_Prototype()
{
    ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
    m_pTriangle = CVI_Triangle::Create(pDevice, "VI_Triangle");
    if (!m_pTriangle)
        return E_FAIL;
    
    return S_OK;
}

HRESULT CTriangleModel::Initialize(COMPONENT_DESC* pArg)
{
    return S_OK;
}

const D3D11_INPUT_ELEMENT_DESC* CTriangleModel::Get_ElementDesc(_uint DrawIndex)
{
    return VTXRECT::Elements;
}

const _uint CTriangleModel::Get_ElementCount(_uint DrawIndex)
{
    return VTXRECT::iElementCount;
}

const string_view CTriangleModel::Get_ElementKey(_uint DrawIndex)
{
    return VTXRECT::Key;
}

HRESULT CTriangleModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
    m_pTriangle->Bind_Buffer(pContext);
    m_pTriangle->Render(pContext);
    return S_OK;
}

HRESULT CTriangleModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
    return S_OK;
}

_uint CTriangleModel::Get_MeshCount()
{
    return 1;
}

_uint CTriangleModel::Get_MaterialIndex(_uint Index)
{
    return 0;
}

_bool CTriangleModel::isDrawable(_uint Index)
{
    return isDrawing;
}

void CTriangleModel::SetDrawable(_uint Index, _bool isDraw)
{
    isDrawing = isDraw;
}

MINMAX_BOX CTriangleModel::Get_LocalBoundingBox()
{
    return MINMAX_BOX{ { -1.f, -1.f, 0.f }, { 1.f, 1.f, 0.f } };
}

MINMAX_BOX CTriangleModel::Get_WorldBoundingBox()
{
    MINMAX_BOX worldBox = {};
    _float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
    XMStoreFloat3(&worldBox.vMin, XMVector3TransformCoord({ -1.f, -1.f, 0.f }, XMLoadFloat4x4(pWorldMat)));
    XMStoreFloat3(&worldBox.vMax, XMVector3TransformCoord({ 1.f,  1.f, 0.f }, XMLoadFloat4x4(pWorldMat)));
    return worldBox;
}

vector<MINMAX_BOX> CTriangleModel::Get_MeshBoundingBoxes()
{
    vector<MINMAX_BOX> boxes;
    boxes.push_back(MINMAX_BOX{ { -1.f, -1.f, 0.f }, { 1.f, 1.f, 0.f } });
    return boxes;
}

MINMAX_BOX CTriangleModel::Get_MeshBoundingBox(_uint index)
{
    return MINMAX_BOX{ { -1.f, -1.f, 0.f }, { 1.f, 1.f, 0.f } };
}

void CTriangleModel::Render_GUI()
{
}

CTriangleModel* CTriangleModel::Create()
{
    CTriangleModel* instance = new CTriangleModel();
    if (FAILED(instance->Initialize_Prototype())) {
        MSG_BOX("CTessellationModel Create Failed");
        Safe_Release(instance);
    }
    return instance;
}

CComponent* CTriangleModel::Clone()
{
    CTriangleModel* instance = new CTriangleModel(*this);
    return instance;
}

void CTriangleModel::Free()
{
    __super::Free();
    Safe_Release(m_pTriangle);
}
