#include "Engine_Defines.h"
#include "TessellationModel.h"
#include "VI_Tessellation.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "GameObject.h"
#include "Transform.h"

CTessellationModel::CTessellationModel()
{
}

CTessellationModel::CTessellationModel(const CTessellationModel& rhs)
    : CModel(rhs), m_pTessBuffer{ rhs.m_pTessBuffer },
    m_fSize{ rhs.m_fSize }, m_iGridSize{ rhs.m_iGridSize }
{
    Safe_AddRef(m_pTessBuffer);
}

HRESULT CTessellationModel::Initialize_Prototype()
{
    ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
    m_pTessBuffer = CVI_Tessellation::Create(pDevice, "VI_Tessellation", m_iGridSize, m_fSize);
    if (!m_pTessBuffer)
        return E_FAIL;

    return S_OK;
}

HRESULT CTessellationModel::Initialize(COMPONENT_DESC* pArg)
{
    return S_OK;
}

const D3D11_INPUT_ELEMENT_DESC* CTessellationModel::Get_ElementDesc(_uint DrawIndex)
{
    return VTXTESS::Elements;
}

const _uint CTessellationModel::Get_ElementCount(_uint DrawIndex)
{
    return VTXTESS::iElementCount;
}

const string_view CTessellationModel::Get_ElementKey(_uint DrawIndex)
{
    return VTXTESS::Key;
}

HRESULT CTessellationModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
    m_pTessBuffer->Bind_Buffer(pContext);
    m_pTessBuffer->Render(pContext);
    pContext->HSSetShader(nullptr, nullptr, 0);
    pContext->DSSetShader(nullptr, nullptr, 0);
    return S_OK;
}

HRESULT CTessellationModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
    return S_OK;
}

_uint CTessellationModel::Get_MeshCount()
{
    return 1;
}

_uint CTessellationModel::Get_MaterialIndex(_uint Index)
{
    return 0;
}

_bool CTessellationModel::isDrawable(_uint Index)
{
    return isDrawing;
}

void CTessellationModel::SetDrawable(_uint Index, _bool isDraw)
{
    isDrawing = isDraw;
}

MINMAX_BOX CTessellationModel::Get_LocalBoundingBox()
{
    _float fHalf = m_fSize * 0.5f;
    return MINMAX_BOX{ { -fHalf, -fHalf, -fHalf }, { fHalf,  fHalf, fHalf } };
}

MINMAX_BOX CTessellationModel::Get_WorldBoundingBox()
{
    _float fHalf = m_fSize * 0.5f;
    MINMAX_BOX worldBox = {};
    _float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
    XMStoreFloat3(&worldBox.vMin, XMVector3TransformCoord({ -fHalf, 0.f, -fHalf }, XMLoadFloat4x4(pWorldMat)));
    XMStoreFloat3(&worldBox.vMax, XMVector3TransformCoord({ fHalf, 0.f, fHalf }, XMLoadFloat4x4(pWorldMat)));
    return worldBox;
}

vector<MINMAX_BOX> CTessellationModel::Get_MeshBoundingBoxes()
{
    vector<MINMAX_BOX> boxes;
    boxes.push_back(Get_LocalBoundingBox());
    return boxes;
}

MINMAX_BOX CTessellationModel::Get_MeshBoundingBox(_uint index)
{
    return Get_LocalBoundingBox();
}

void CTessellationModel::Render_GUI()
{
}

CTessellationModel* CTessellationModel::Create(_uint iGridSize, _float fTotalSize)
{
    CTessellationModel* instance = new CTessellationModel();
    instance->m_iGridSize = iGridSize;
    instance->m_fSize = fTotalSize;
    if (FAILED(instance->Initialize_Prototype())) {
        MSG_BOX("CTessellationModel Create Failed");
        Safe_Release(instance);
    }
    return instance;
}

CComponent* CTessellationModel::Clone()
{
    CTessellationModel* instance = new CTessellationModel(*this);
    return instance;
}

void CTessellationModel::Free()
{
    __super::Free();
    Safe_Release(m_pTessBuffer);
}