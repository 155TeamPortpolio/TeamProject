#include "Engine_Defines.h"
#include "StaticMeshBatcher.h"
#include "Model.h"
#include "Material.h"
#include "RenderPass.h"
#include "Renderer.h"
#include "Shader.h"

CStaticMeshBatcher::CStaticMeshBatcher()
{
}

HRESULT CStaticMeshBatcher::Initialize(ID3D11Device* device, _uint maxInstancesPerBatch)
{
	Safe_AddRef(device);
	m_pDevice = device;   
	m_maxInstances = (maxInstancesPerBatch == 0) ? 1u : maxInstancesPerBatch;
	EnsureInstanceIndexBuffer();
	return S_OK;
}

void CStaticMeshBatcher::Submit(const OPAQUE_PACKET& packet)
{
	if (!packet.pModel || !packet.pMaterial)
		return;

	BatchKey key{};
	key.pModel = packet.pModel;
	key.pMaterial = packet.pMaterial;
	key.drawIndex = packet.DrawIndex;
	key.materialIndex = packet.MaterialIndex;
	key.passConstant = packet.pMaterial->GetPassConstant(packet.MaterialIndex);

	auto& group = m_Batchgroups[key];
	group.key = key;
	group.transformIndices.push_back(packet.TransformIndex);
}

void CStaticMeshBatcher::BeginFrame()
{
	m_Batchgroups.clear();
}

void CStaticMeshBatcher::Flush(ID3D11DeviceContext* context, RenderPass* pRenderPass, CRenderer* pRenderer)
{
	if (!context)
		return;

	EnsureInstanceIndexBuffer();
	if (!m_instanceIndexSRV)
		return;

	for (auto& pair : m_Batchgroups)
	{
		BatchGroup& group = pair.second;
		if (group.transformIndices.empty())
			continue;

		// 너무 많으면 쪼개서 여러 번 드로우
		_uint startIndex = 0;
		const _uint totalCount = static_cast<_uint>(group.transformIndices.size());

		while (startIndex < totalCount)
		{
			const _uint batchCount = min(m_maxInstances, totalCount - startIndex);

			UploadInstanceIndices(context, group.transformIndices.data() + startIndex, batchCount);
			pRenderPass->BindConstant(context,
				group.key.pModel,
				group.key.pMaterial,
				group.key.drawIndex,
				group.key.materialIndex,
				pRenderer);
			SHADER_PARAM instParam{};
			instParam.iSize = sizeof(uint32_t) * batchCount;
			instParam.typeName = "StructuredBuffer";
			instParam.pData = m_instanceIndexSRV; 
			pRenderPass->Get_CurShader()->Bind_Value("g_InstanceTransformIndex", instParam);

	
			group.key.pMaterial->Apply_Material(context, group.key.materialIndex);

			group.key.pModel->Draw_Instanced(context, group.key.drawIndex, batchCount);
			group.key.pMaterial->ResetMaterial(group.key.drawIndex);
			startIndex += batchCount;
		}
	}
}

void CStaticMeshBatcher::EnsureInstanceIndexBuffer()
{
	if (m_instanceIndexBuffer && m_instanceIndexSRV)
		return;

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(uint32_t) * m_maxInstances;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_instanceIndexBuffer);
	if (FAILED(hr))
		return;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Structured는 UNKNOWN
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_maxInstances;

	hr = m_pDevice->CreateShaderResourceView(m_instanceIndexBuffer, &srvDesc, &m_instanceIndexSRV);
	if (FAILED(hr))
		return;
}

void CStaticMeshBatcher::UploadInstanceIndices(ID3D11DeviceContext* context, const uint32_t* indices, uint32_t count)
{
	if (!context || !m_instanceIndexBuffer || !indices || count == 0)
		return;

	D3D11_MAPPED_SUBRESOURCE mapped{};
	HRESULT hr = context->Map(m_instanceIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
		return;

	memcpy(mapped.pData, indices, sizeof(uint32_t) * count);
	context->Unmap(m_instanceIndexBuffer, 0);
}

CStaticMeshBatcher* CStaticMeshBatcher::Create(ID3D11Device* device, _uint maxInstancesPerBatch)
{
	CStaticMeshBatcher* instance = new CStaticMeshBatcher();
	if (FAILED(instance->Initialize(device, maxInstancesPerBatch))) {
		Safe_Release(instance);
	}
	return instance;
}
void CStaticMeshBatcher::Free()
{
	Safe_Release(m_pDevice);
	__super::Free();
}
