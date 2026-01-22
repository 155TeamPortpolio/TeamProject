#include "Engine_Defines.h"
#include "CellBatcher.h"
#include "Shader.h"
#include "Material.h"
#include "StaticModel.h"
#include "ModelData.h"
#include "Mesh.h"
#include "RenderSystem.h"
#include "PipeLine.h"

CCellBatcher::CCellBatcher(CRenderSystem* pRenderSys, const Options& opt)
	:m_Options{ opt },m_pRenderSystem{pRenderSys}
{
}

CellKey CCellBatcher::MakeCellKey(const _float4x4& worldMatrix, class CModel* pModel, _uint drawIndex) const
{
	MINMAX_BOX localBox = pModel->Get_MeshBoundingBox(drawIndex);
	MINMAX_BOX worldBox = localBox.TransformBox_8Corner(worldMatrix);

	_float3 center;
	center.x = (worldBox.vMin.x + worldBox.vMax.x) * 0.5f;
	center.y = (worldBox.vMin.y + worldBox.vMax.y) * 0.5f;
	center.z = (worldBox.vMin.z + worldBox.vMax.z) * 0.5f;

	CellKey key;
	key.x = (_int)floorf(center.x / m_Options.cellSize);
	key.z = (_int)floorf(center.z / m_Options.cellSize);
	return key;
}

bool CCellBatcher::CanBatch(const OPAQUE_PACKET& packet) const
{
	if (!packet.pModel || !packet.pMaterial || !packet.pWorldMatrix) return false;
	if (packet.bSkinning) return false; // static pass�� ������ġ
	return true;
}

bool CCellBatcher::BuildOneBatch(ID3D11Device* device, const CellBatchKey& key, const vector<OPAQUE_PACKET*>& packets)
{
	if (!device) return false;
	if (packets.size() < m_Options.minBatchCount) return false;

	vector<VTXMESH> mergedVertices;
	vector<_uint> mergedIndices;

	// �뷫 reserve(��Ȯ�� �ջ��Ϸ��� �ѹ� �� ����)
	mergedVertices.reserve(packets.size() * 256);
	mergedIndices.reserve(packets.size() * 512);

	MINMAX_BOX aabb{};
	aabb.vMin = _float3{ FLT_MAX, FLT_MAX, FLT_MAX };
	aabb.vMax = _float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (const OPAQUE_PACKET* packetPtr : packets)
	{
		const OPAQUE_PACKET& packet = *packetPtr;
		auto pData = static_cast<CStaticModel*>(packet.pModel)->Get_ModelData();
		const vector<VTXMESH>& srcVertices = pData->Get_Mesh(packet.DrawIndex)->Get_StaticVertices();
		const vector<_uint>& srcIndices = pData->Get_Mesh(packet.DrawIndex)->Get_Indices();

		AppendWorldBaked(srcVertices, srcIndices, *packet.pWorldMatrix, mergedVertices, mergedIndices);

		MINMAX_BOX localBox = pData->Get_MeshBoundingBox(packet.DrawIndex);
		localBox.TransformBox_8Corner(*packet.pWorldMatrix);

		aabb.vMin.x = min(aabb.vMin.x, localBox.vMin.x);
		aabb.vMin.y = min(aabb.vMin.y, localBox.vMin.y);
		aabb.vMin.z = min(aabb.vMin.z, localBox.vMin.z);
		aabb.vMax.x = max(aabb.vMax.x, localBox.vMax.x);
		aabb.vMax.y = max(aabb.vMax.y, localBox.vMax.y);
		aabb.vMax.z = max(aabb.vMax.z, localBox.vMax.z);
	}

	// GPU VB/IB ����
	ID3D11Buffer* VB = nullptr;
	ID3D11Buffer* IB = nullptr;

	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = (UINT)(mergedVertices.size() * sizeof(VTXMESH));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sub{};
		sub.pSysMem = mergedVertices.data();

		if (FAILED(device->CreateBuffer(&desc, &sub, &VB)))
			return false;
	}
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = (UINT)(mergedIndices.size() * sizeof(_uint));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sub{};
		sub.pSysMem = mergedIndices.data();

		if (FAILED(device->CreateBuffer(&desc, &sub, &IB)))
		{
			Safe_Release(VB);
			return false;
		}
	}

	// ĳ�� ����(���� ������ ��ü)
	CachedBatch batch{};
	batch.pVB = VB;
	batch.pIB = IB;
	batch.indexCount = (_uint)mergedIndices.size();
	batch.material = key.batch.pMaterial;
	batch.materialIndex = key.batch.iMaterialIndex;
	batch.shader = key.batch.pShader;
	batch.lastUsedFrame = m_iFrameIndex;
	batch.worldAabb = aabb;
	batch.vertexStride = sizeof(VTXMESH);

	auto found = m_Cached.find(key);
	if (found != m_Cached.end())
	{
		Safe_Release(found->second.pVB);
		Safe_Release(found->second.pIB);
		found->second = batch;
	}
	else
	{
		m_Cached.emplace(key, batch);
	}

	return true;
}

CCellBatcher::CellRange CCellBatcher::MakeCellRange(const _float4x4& worldMatrix, CModel* pModel, _uint drawIndex) const
{
	MINMAX_BOX localBox = pModel->Get_MeshBoundingBox(drawIndex);
	MINMAX_BOX worldBox = localBox.TransformBox_8Corner(worldMatrix);

	CellRange range{};
	range.minX = (int)floorf(worldBox.vMin.x / m_Options.cellSize);
	range.maxX = (int)floorf(worldBox.vMax.x / m_Options.cellSize);
	range.minZ = (int)floorf(worldBox.vMin.z / m_Options.cellSize);
	range.maxZ = (int)floorf(worldBox.vMax.z / m_Options.cellSize);
	return range;
}

void CCellBatcher::BeginBatchFrame(_uint frameIndex)
{
	m_iFrameIndex = frameIndex;
	m_BatchGroups.clear();
}

void CCellBatcher::SubmitVisiblePacket(OPAQUE_PACKET& packet)
{
	if (!CanBatch(packet)) return;

	CShader* shader = packet.pMaterial->Get_Shader(packet.MaterialIndex);
	if (!shader) return;

	BatchingKey batch;
	batch.pMaterial = packet.pMaterial;
	batch.iMaterialIndex = packet.MaterialIndex;
	batch.pShader = shader;

	CellRange range = MakeCellRange(*packet.pWorldMatrix, packet.pModel, packet.DrawIndex);

	for (int cellX = range.minX; cellX <= range.maxX; ++cellX)
	{
		for (int cellZ = range.minZ; cellZ <= range.maxZ; ++cellZ)
		{
			CellKey cell;
			cell.x = cellX;
			cell.z = cellZ;

			CellBatchKey key{ cell, batch };
			m_BatchGroups[key].push_back(&packet);
		}
	}
}

void CCellBatcher::BuildBatchesIfNeeded(ID3D11Device* device)
{
	_uint built = 0;

	for (auto& pair : m_BatchGroups)
	{
		if (built >= m_Options.maxBuildPerFrame)
			break;

		const CellBatchKey& key = pair.first;
		const auto& packets = pair.second;

		if (m_Cached.find(key) != m_Cached.end())
			continue;

		if (BuildOneBatch(device, key, packets))
			built += 1;
	}
}

void CCellBatcher::DrawBatches(ID3D11DeviceContext* context, RenderPass* pass, CRenderer* renderer)
{
	if (!context) return;

	ID3D11Device* device = nullptr;
	context->GetDevice(&device);
	if (!device) return;

	for (auto& pair : m_Cached)
	{
		const CellBatchKey& key = pair.first;
		CachedBatch& batch = pair.second;

		auto groupIt = m_BatchGroups.find(key);
		if (groupIt == m_BatchGroups.end())
			continue;

		if (!batch.shader || !batch.material)
			continue;

		ID3D11InputLayout* layout = GetOrCreateBatchInputLayout(device, batch.shader, "Opaque");
		if (!layout)
			continue;

		context->IASetInputLayout(layout);

		// TransformIndex = 0 (identity)
		_uint identityIndex = 0;
		SHADER_PARAM worldIdx{ &identityIndex, "uint", sizeof(UINT) };
		batch.shader->Bind_Value("TransformIndex", worldIdx);

		// VB/IB bind
		UINT stride = batch.vertexStride;
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &batch.pVB, &stride, &offset);
		context->IASetIndexBuffer(batch.pIB, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CPipeLine* pipe = m_pRenderSystem->Get_Pipeline();
		batch.shader->SetConstantBuffer("FrameBuffer", pipe->Get_FrameBuffer());
		batch.shader->Bind_Value("ObjectBufferArray", { pipe->Get_ObjectResource(), "StructuredBuffer", sizeof(_float4x4)*g_iMaxTransform });

		batch.shader->Apply("Opaque", context);
		batch.material->Apply_Material(context, batch.materialIndex);

		context->DrawIndexed(batch.indexCount, 0, 0);

		batch.material->ResetMaterial(0);
		batch.lastUsedFrame = m_iFrameIndex;
		for (OPAQUE_PACKET* packetPtr : groupIt->second)
		{
			if (packetPtr)
				packetPtr->isBatched = true;
		}
	}

	Safe_Release(device);
}


void CCellBatcher::EndBatchFrame()
{
	m_BatchGroups.clear();
}

void CCellBatcher::Clear()
{
	m_BatchGroups.clear();

	for (auto& it : m_LayoutCache)
		Safe_Release(it.second);
	m_LayoutCache.clear();
}

void CCellBatcher::AppendWorldBaked(const vector<VTXMESH>& srcVertices, const vector<_uint>& srcIndices, const _float4x4& world, vector<VTXMESH>& outVertices, vector<_uint>& outIndices)
{
	//
	if (srcVertices.empty() || srcIndices.empty())
		return;

	const _uint baseVertex = (_uint)outVertices.size();

	// DirectXMath
	_matrix worldMat = XMLoadFloat4x4(&world);

	// normal/tangent ��ȯ��: inverse-transpose(3x3)
	// - world�� translation ����
	_matrix worldNoT = worldMat;
	worldNoT.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	// inverse-transpose
	_matrix invWorld3x3 = XMMatrixInverse(nullptr, worldNoT);
	_matrix normalMat = XMMatrixTranspose(invWorld3x3);

	outVertices.reserve(outVertices.size() + srcVertices.size());
	outIndices.reserve(outIndices.size() + srcIndices.size());


	for (const VTXMESH& v : srcVertices)
	{
		VTXMESH baked = v;

		// POSITION (w=1)
		_vector p = XMVectorSet(v.vPosition.x, v.vPosition.y, v.vPosition.z, 1.f);
		_vector pW = XMVector4Transform(p, worldMat);
		XMStoreFloat3(&baked.vPosition, pW);

		// NORMAL (w=0) + normalize
		_vector n = XMVectorSet(v.vNormal.x, v.vNormal.y, v.vNormal.z, 0.f);
		_vector nW = XMVector3TransformNormal(n, normalMat);
		nW = XMVector3Normalize(nW);
		XMStoreFloat3(&baked.vNormal, nW);

		// TANGENT (w=0) + normalize
		_vector t = XMVectorSet(v.vTangent.x, v.vTangent.y, v.vTangent.z, 0.f);
		_vector tW = XMVector3TransformNormal(t, normalMat);
		tW = XMVector3Normalize(tW);
		XMStoreFloat3(&baked.vTangent, tW);

		// BINORMAL (w=0) + normalize
		_vector b = XMVectorSet(v.vBinormal.x, v.vBinormal.y, v.vBinormal.z, 0.f);
		_vector bW = XMVector3TransformNormal(b, normalMat);
		bW = XMVector3Normalize(bW);
		XMStoreFloat3(&baked.vBinormal, bW);

		outVertices.push_back(baked);
	}

	for (_uint idx : srcIndices)
	{
		outIndices.push_back(idx + baseVertex);
	}
}

ID3D11InputLayout* CCellBatcher::GetOrCreateBatchInputLayout(
	ID3D11Device* device,
	CShader* shader,
	const char* passName)
{
	if (!device || !shader || !passName)
		return nullptr;

	const string key = string(VTXMESH::Key) + "|" + shader->Get_Key() + "|" + passName; 

	auto found = m_LayoutCache.find(key);
	if (found != m_LayoutCache.end() && found->second)
		return found->second;

	D3DX11_PASS_DESC passDesc{};
	if (FAILED(shader->GetPassSignature(passName, &passDesc)))
		return nullptr;

	ID3D11InputLayout* layout = nullptr;

	HRESULT hr = device->CreateInputLayout(
		VTXMESH::Elements, VTXMESH::iElementCount,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&layout
	);
	if (FAILED(hr) || !layout)
		return nullptr;

	m_LayoutCache.emplace(key, layout);
	return layout;
}

CCellBatcher* CCellBatcher::Create(CRenderSystem* pRenderSys, const Options& opt)
{
	CCellBatcher* Instance = new CCellBatcher(pRenderSys,opt);
	return Instance;
}

void CCellBatcher::Free() {
	__super::Free();
	Clear();
}
