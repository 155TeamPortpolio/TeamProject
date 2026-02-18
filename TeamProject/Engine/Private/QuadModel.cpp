#include "Engine_Defines.h"
#include "QuadModel.h"
#include "VI_Quad.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "GameObject.h"
#include "Transform.h"

CQuadModel::CQuadModel()
{
}

CQuadModel::CQuadModel(const CQuadModel& rhs)
    :CModel(rhs), m_pQuad{ rhs.m_pQuad }
{
    Safe_AddRef(m_pQuad);
}

HRESULT CQuadModel::Initialize_Prototype()
{
    ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
    m_pQuad = CVI_Quad::Create(pDevice, "VI_Quad");
    if (!m_pQuad)
        return E_FAIL;
    
    return S_OK;
}

HRESULT CQuadModel::Initialize(COMPONENT_DESC* pArg)
{
    return S_OK;
}

const D3D11_INPUT_ELEMENT_DESC* CQuadModel::Get_ElementDesc(_uint DrawIndex)
{
    return VTXRECT::Elements;
}

const _uint CQuadModel::Get_ElementCount(_uint DrawIndex)
{
    return VTXRECT::iElementCount;
}

const string_view CQuadModel::Get_ElementKey(_uint DrawIndex)
{
    return VTXRECT::Key;
}

HRESULT CQuadModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
    m_pQuad->Bind_Buffer(pContext);
    m_pQuad->Render(pContext);
    return S_OK;
}

HRESULT CQuadModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
    return S_OK;
}

_uint CQuadModel::Get_MeshCount()
{
    return 1;
}

_uint CQuadModel::Get_MaterialIndex(_uint Index)
{
    return 0;
}

_bool CQuadModel::isDrawable(_uint Index)
{
    return isDrawing;
}

void CQuadModel::SetDrawable(_uint Index, _bool isDraw)
{
    isDrawing = isDraw;
}

MINMAX_BOX CQuadModel::Get_LocalBoundingBox()
{
    return MINMAX_BOX{ { -1.f, -1.f, 0.f }, { 1.f, 1.f, 0.f } };
}

MINMAX_BOX CQuadModel::Get_WorldBoundingBox()
{
    MINMAX_BOX worldBox = {};
    _float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
    XMStoreFloat3(&worldBox.vMin, XMVector3TransformCoord({ -1.f, -1.f, 0.f }, XMLoadFloat4x4(pWorldMat)));
    XMStoreFloat3(&worldBox.vMax, XMVector3TransformCoord({ 1.f,  1.f, 0.f }, XMLoadFloat4x4(pWorldMat)));
    return worldBox;
}

vector<MINMAX_BOX> CQuadModel::Get_MeshBoundingBoxes()
{
    vector<MINMAX_BOX> boxes;
    boxes.push_back(MINMAX_BOX{ { -1.f, -1.f, 0.f }, { 1.f, 1.f, 0.f } });
    return boxes;
}

MINMAX_BOX CQuadModel::Get_MeshBoundingBox(_uint index)
{
    return MINMAX_BOX{ { -1.f, -1.f, 0.f }, { 1.f, 1.f, 0.f } };
}

void CQuadModel::Render_GUI()
{
}

CQuadModel* CQuadModel::Create()
{
    CQuadModel* instance = new CQuadModel();
    if (FAILED(instance->Initialize_Prototype())) {
        MSG_BOX("CTessellationModel Create Failed");
        Safe_Release(instance);
    }
    return instance;
}

CComponent* CQuadModel::Clone()
{
    CQuadModel* instance = new CQuadModel(*this);
    return instance;
}

void CQuadModel::Free()
{
    __super::Free();
    Safe_Release(m_pQuad);
}
