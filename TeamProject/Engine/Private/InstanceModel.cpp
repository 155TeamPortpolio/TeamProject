#include "InstanceModel.h"
#include "VIBuffer.h"
#include "Mesh.h"
#include	"ModelData.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "GameObject.h"
#include "Transform.h"
CInstanceModel::CInstanceModel()
{
}

CInstanceModel::CInstanceModel(const CInstanceModel& rhs)
	:CModel(rhs)
{
}

CInstanceModel::~CInstanceModel()
{
}


HRESULT CInstanceModel::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

HRESULT CInstanceModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInstanceModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
	Safe_Release(m_pModelData);
	m_pModelData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(levelKey, modelDataKey);
	Safe_AddRef(m_pModelData);
	m_DrawableMeshes.resize(m_pModelData->Get_MeshCount(), true);

	if (m_pModelData == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Link_InstanceData(ID3D11Device* pDevice, vector<INSTANCE_INIT_DESC> initArray, const string& LevelKey, const string& ModelKey)
{
	HRESULT result = {};
	Safe_Release(m_pModelData);
	m_pModelData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(LevelKey, ModelKey);
	Safe_AddRef(m_pModelData);
	m_DrawableMeshes.resize(m_pModelData->Get_MeshCount(), true);
	m_StringViews.resize(m_pModelData->Get_MeshCount());
	m_CombinedViews.resize(m_pModelData->Get_MeshCount());
	if (m_pModelData == nullptr)
		result = E_FAIL;

	_uint count = initArray.size();

	m_InstanceBuffers.resize(count);
	m_ActiveCounts.resize(count);
	m_DescArray.resize(count);

	for (size_t i = 0; i < count; i++)
	{
		m_DescArray[i] = initArray[i];

		D3D11_BUFFER_DESC           InstanceDesc{};
		InstanceDesc.ByteWidth = m_DescArray[i].instanceCount * m_DescArray[i].instanceStride;
		InstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
		InstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		InstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		InstanceDesc.MiscFlags = 0;
		InstanceDesc.StructureByteStride = m_DescArray[i].instanceStride;

		if (FAILED(pDevice->CreateBuffer(&InstanceDesc, nullptr, &m_InstanceBuffers[i])))
			result = E_FAIL;
	}

	if (result == E_FAIL) {

		for (auto& buffer : m_InstanceBuffers)
		{
			Safe_Release(buffer);
		}
		Safe_Release(m_pModelData);

		return E_FAIL;
	}

	return S_OK;
}

void CInstanceModel::Link_InstanceWithMesh(_uint MeshIndex, _uint InstanceIndex)
{
	m_MapMeshWithInstance.emplace(MeshIndex, InstanceIndex);
}

void CInstanceModel::Link_InstanceMeshAll(_uint InstanceIndex)
{
	for (size_t i = 0; i < m_pModelData->Get_MeshCount(); i++)
	{
		m_MapMeshWithInstance[i] = InstanceIndex;
	}
}

HRESULT CInstanceModel::Update_Instance(ID3D11DeviceContext* pContext, const void* pData, _uint Index, _uint count)
{
	if (!Check_Valid(Index)) {
		return E_FAIL;
	}
 	if (Index >= m_ActiveCounts.size()) return E_FAIL;


	if (!pContext || !pData || m_InstanceBuffers.empty())
		return E_FAIL;

	m_ActiveCounts[Index] = count;

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	_uint stride = m_DescArray[Index].instanceStride;

	pContext->Map(m_InstanceBuffers[Index], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	//mappedResource.pData 이 안에 지금 데이터 정보들어가 있음.
	memcpy(mappedResource.pData, pData, stride * m_ActiveCounts[Index]);
	pContext->Unmap(m_InstanceBuffers[Index], 0);

	return S_OK;
}

HRESULT CInstanceModel::Bind_Buffer(ID3D11DeviceContext* pContext, _uint MeshIndex)
{
	if (!Check_Valid(MeshIndex)) {
		return E_FAIL;
	}
	CVIBuffer* pBuffer = m_pModelData->Get_Mesh(MeshIndex);
	_uint InstanceIndex = m_MapMeshWithInstance[MeshIndex];

	UINT strides[2] = { pBuffer->Get_VertexStride(),m_DescArray[InstanceIndex].instanceStride };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer* buffers[2] = { pBuffer->Get_VertexBuffer(), m_InstanceBuffers[InstanceIndex] };

	pContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
	pContext->IASetIndexBuffer(pBuffer->Get_IndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	return S_OK;
}

HRESULT CInstanceModel::Draw(ID3D11DeviceContext* pContext, _uint MeshIndex)
{
	if (!Check_Valid(MeshIndex)) {
		return E_FAIL;
	}
	CVIBuffer* pBuffer = m_pModelData->Get_Mesh(MeshIndex);
	_uint InstanceIndex = m_MapMeshWithInstance[MeshIndex];
	pContext->DrawIndexedInstanced(pBuffer->Get_IndexCount(), m_ActiveCounts[InstanceIndex], 0, 0, 0);
	return S_OK;
}

const D3D11_INPUT_ELEMENT_DESC* CInstanceModel::Get_ElementDesc(_uint meshIndex)
{
	_uint InstanceIndex = m_MapMeshWithInstance[meshIndex];
	const D3D11_INPUT_ELEMENT_DESC* modelElems = m_pModelData->Get_ElementDesc(meshIndex);
	const D3D11_INPUT_ELEMENT_DESC* instElems = m_DescArray[InstanceIndex].pElementDesc;

	UINT modelCount = m_pModelData->Get_ElementCount(meshIndex);
	UINT instCount = m_DescArray[InstanceIndex].ElementCount;

	m_CombinedDescs[meshIndex].resize(modelCount + instCount);
	memcpy(m_CombinedDescs[meshIndex].data(), modelElems, sizeof(D3D11_INPUT_ELEMENT_DESC) * modelCount);
	memcpy(m_CombinedDescs[meshIndex].data() + modelCount, instElems, sizeof(D3D11_INPUT_ELEMENT_DESC) * instCount);

	return m_CombinedDescs[meshIndex].data();
}

const _uint CInstanceModel::Get_ElementCount(_uint meshIndex)
{
	_uint InstanceIndex = m_MapMeshWithInstance[meshIndex];
	return m_pModelData->Get_ElementCount(meshIndex) + m_DescArray[InstanceIndex].ElementCount;
}


const string_view CInstanceModel::Get_ElementKey(_uint meshIndex)
{

	if (m_StringViews[meshIndex].empty()) {
		_uint InstanceIndex = m_MapMeshWithInstance[meshIndex];
		string instanceKey = m_DescArray[InstanceIndex].ElementKey;
		string meshKey = string(m_pModelData->Get_ElementKey(meshIndex));  

		m_CombinedViews[meshIndex] = instanceKey + "_" + meshKey;
		m_StringViews[meshIndex] = m_CombinedViews[meshIndex];
	}

	return m_StringViews[meshIndex];
}

_uint CInstanceModel::Get_MeshCount()
{
	return m_pModelData->Get_MeshCount();
}

_uint CInstanceModel::Get_MaterialIndex(_uint Index)
{
	return m_pModelData->Get_MaterialIndex(Index);
}

_bool CInstanceModel::isDrawable(_uint Index)
{
	return m_DrawableMeshes[Index];
}

void CInstanceModel::SetDrawable(_uint Index, _bool isDraw)
{
	if (Index >= m_DrawableMeshes.size())
		return;

	m_DrawableMeshes[Index] = isDraw;
}

_bool CInstanceModel::isReadyToDraw()
{
	if (!m_pModelData)
		return false;

	if (m_InstanceBuffers.empty())
		return false;

	return true;
}

MINMAX_BOX CInstanceModel::Get_LocalBoundingBox()
{
	return m_pModelData->Get_LocalBoundingBox();
}

MINMAX_BOX CInstanceModel::Get_WorldBoundingBox()
{
	MINMAX_BOX wordlBox = m_pModelData->Get_LocalBoundingBox();
	_float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
	XMStoreFloat3(&wordlBox.vMin, XMVector3TransformCoord(XMLoadFloat3(&wordlBox.vMin), XMLoadFloat4x4(pWorldMat)));
	XMStoreFloat3(&wordlBox.vMax, XMVector3TransformCoord(XMLoadFloat3(&wordlBox.vMax), XMLoadFloat4x4(pWorldMat)));
	return wordlBox;
}

MINMAX_BOX CInstanceModel::Get_MeshBoundingBox(_uint index)
{
	return m_pModelData->Get_LocalBoundingBox();
}

_bool CInstanceModel::Check_Valid(_uint MeshIndex)
{
	auto iter = m_MapMeshWithInstance.find(MeshIndex);
	if (iter == m_MapMeshWithInstance.end())
		return false;
	return true;
}

vector<MINMAX_BOX> CInstanceModel::Get_MeshBoundingBox()
{
	vector<MINMAX_BOX> boxes;

	for (size_t i = 0; i < m_pModelData->Get_MeshCount(); i++)
	{
		boxes.push_back(m_pModelData->Get_MeshBoundingBox(i));
	}
	return boxes;
}

CInstanceModel* CInstanceModel::Create()
{
	CInstanceModel* instance = new CInstanceModel();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CInstanceModel Create Failed : CInstanceModel");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CInstanceModel::Clone()
{
	CInstanceModel* instance = new CInstanceModel(*this);
	return instance;
}

void CInstanceModel::Free()
{
	__super::Free();
	Safe_Release(m_pModelData);

	for (auto& buffer : m_InstanceBuffers)
	{
		Safe_Release(buffer);
	}
}