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

static MergeSizes ComputeMergeSizes(const vector<OPAQUE_PACKET*>& packets)
{
	MergeSizes sizes{};
	for (const OPAQUE_PACKET* packetPtr : packets)
	{
		if (!packetPtr || !packetPtr->pModel) continue;

		const OPAQUE_PACKET& packet = *packetPtr;
		auto* staticModel = static_cast<CStaticModel*>(packet.pModel);
		auto* modelData = staticModel->Get_ModelData();
		if (!modelData) continue;

		auto* mesh = modelData->Get_Mesh(packet.DrawIndex);
		if (!mesh) continue;

		sizes.vertexCount += mesh->Get_StaticVertices().size();
		sizes.indexCount += mesh->Get_Indices().size();
	}
	return sizes;
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
	const MergeSizes mergeSizes = ComputeMergeSizes(packets);

	vector<VTXMESH> mergedVertices;
	vector<_uint> mergedIndices;

	mergedVertices.reserve(mergeSizes.vertexCount);
	mergedIndices.reserve(mergeSizes.indexCount);

	MINMAX_BOX aabb{};
	aabb.vMin = _float3{ FLT_MAX, FLT_MAX, FLT_MAX };
	aabb.vMax = _float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (const OPAQUE_PACKET* packetPtr : packets)
	{
		if (!packetPtr) continue;
		const OPAQUE_PACKET& packet = *packetPtr;

		auto* staticModel = static_cast<CStaticModel*>(packet.pModel);
		auto* modelData = staticModel ? staticModel->Get_ModelData() : nullptr;
		if (!modelData) continue;

		 auto* mesh = modelData->Get_Mesh(packet.DrawIndex);
		if (!mesh) continue;

		 const vector<VTXMESH>& srcVertices = mesh->Get_StaticVertices();
		 const vector<_uint>& srcIndices = mesh->Get_Indices();

		AppendWorldBaked(srcVertices, srcIndices, *packet.pWorldMatrix, mergedVertices, mergedIndices);

		MINMAX_BOX localBox = modelData->Get_MeshBoundingBox(packet.DrawIndex);
		MINMAX_BOX worldBox = localBox.TransformBox_8Corner(*packet.pWorldMatrix);

		aabb.vMin.x = min(aabb.vMin.x, worldBox.vMin.x);
		aabb.vMin.y = min(aabb.vMin.y, worldBox.vMin.y);
		aabb.vMin.z = min(aabb.vMin.z, worldBox.vMin.z);
		aabb.vMax.x = max(aabb.vMax.x, worldBox.vMax.x);
		aabb.vMax.y = max(aabb.vMax.y, worldBox.vMax.y);
		aabb.vMax.z = max(aabb.vMax.z, worldBox.vMax.z);
	}

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = (UINT)(mergedVertices.size() * sizeof(VTXMESH));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sub{};
		sub.pSysMem = mergedVertices.data();

		if (FAILED(device->CreateBuffer(&desc, &sub, &vertexBuffer)))
			return false;
	}
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = (UINT)(mergedIndices.size() * sizeof(_uint));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sub{};
		sub.pSysMem = mergedIndices.data();

		if (FAILED(device->CreateBuffer(&desc, &sub, &indexBuffer)))
		{
			Safe_Release(vertexBuffer);
			return false;
		}
	}

	CachedBatch batch{};
	batch.pVB = vertexBuffer;
	batch.pIB = indexBuffer;
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
	m_BatchGroups.clear(); // 여기서만 clear
	TrimCache();
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
	if (!device) return;

	_uint builtCount = 0;

	for (auto& pair : m_BatchGroups)
	{
		if (builtCount >= m_Options.maxBuildPerFrame)
			break;

		const CellBatchKey& key = pair.first;
		const vector<OPAQUE_PACKET*>& packets = pair.second;

		// 배치 조건이 안 맞으면 스킵
		if (packets.size() < m_Options.minBatchCount)
			continue;

		const uint64_t currentHash = ComputeGroupHash(packets);

		auto cachedIt = m_Cached.find(key);
		if (cachedIt != m_Cached.end())
		{
			CachedBatch& cachedBatch = cachedIt->second;

			const bool sameCount = (cachedBatch.memberCount == (_uint)packets.size());
			const bool sameHash = (cachedBatch.memberHash == currentHash);

			if (sameCount && sameHash)
				continue;
		}

		if (BuildOneBatch(device, key, packets))
		{
			auto updatedIt = m_Cached.find(key);
			if (updatedIt != m_Cached.end())
			{
				updatedIt->second.memberHash = currentHash;
				updatedIt->second.memberCount = (_uint)packets.size();
			}

			builtCount += 1;
		}
	}
}

_uint CCellBatcher::DrawBatches(ID3D11DeviceContext* context, RenderPass* pass, CRenderer* renderer)
{
	if (!context) return 0;

	ID3D11Device* device = nullptr;
	context->GetDevice(&device);
	if (!device) return 0;

	_uint drawnBatchCount = 0;

	for (auto& pair : m_Cached)
	{
		const CellBatchKey& key = pair.first;
		CachedBatch& batch = pair.second;

		auto groupIt = m_BatchGroups.find(key);
		if (groupIt == m_BatchGroups.end())
			continue;

		const vector<OPAQUE_PACKET*>& packets = groupIt->second;

		// ✅ 중요: 캐시가 "현재 그룹"과 동일한 멤버인지 확인
		const uint64_t currentHash = ComputeGroupHash(packets);
		const bool upToDate =
			(batch.memberCount == (_uint)packets.size()) &&
			(batch.memberHash == currentHash);

		if (!upToDate)
		{
			// 캐시가 낡았으면 이번 프레임엔 배치로 그리지 말고,
			// non-batch 경로로 fallback 하게 둬야 깜빡임이 사라짐.
			continue;
		}

		if (!batch.shader || !batch.material || !batch.pVB || !batch.pIB)
			continue;

		ID3D11InputLayout* layout = GetOrCreateBatchInputLayout(device, batch.shader, "Opaque");
		if (!layout)
			continue;

		context->IASetInputLayout(layout);

		// TransformIndex=0 (Begin_ObjectBuffer에서 [0]=identity 보장)
		_uint identityIndex = 0;
		SHADER_PARAM worldIdx{ &identityIndex, "uint", sizeof(UINT) };
		batch.shader->Bind_Value("TransformIndex", worldIdx);

		UINT stride = batch.vertexStride;
		UINT offset = 0;
		ID3D11Buffer* vertexBuffer = batch.pVB;
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(batch.pIB, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CPipeLine* pipe = m_pRenderSystem->Get_Pipeline();
		batch.shader->SetConstantBuffer("FrameBuffer", pipe->Get_FrameBuffer());
		batch.shader->Bind_Value(
			"ObjectBufferArray",
			{ pipe->Get_ObjectResource(), "StructuredBuffer", sizeof(_float4x4) * g_iMaxTransform }
		);

		// 패스 적용이 필요한 엔진이면 여기서 반드시 Apply 해라 (너는 주석처리해둠)
		// batch.shader->Apply("Opaque", context);

		batch.material->Apply_Material(context, batch.materialIndex);

		context->DrawIndexed(batch.indexCount, 0, 0);
		drawnBatchCount++;

		batch.material->ResetMaterial(0);
		batch.lastUsedFrame = m_iFrameIndex;

		for (OPAQUE_PACKET* packetPtr : packets)
		{
			if (packetPtr) packetPtr->isBatched = true;
		}
	}

	Safe_Release(device);
	return drawnBatchCount;
}


void CCellBatcher::TrimCache()
{
	if (m_Options.keepCachedFrames == 0) return;

	const _uint limit = m_Options.keepCachedFrames;
	for (auto it = m_Cached.begin(); it != m_Cached.end(); )
	{
		CachedBatch& batch = it->second;
		if ((m_iFrameIndex - batch.lastUsedFrame) > limit)
		{
			Safe_Release(batch.pVB);
			Safe_Release(batch.pIB);
			it = m_Cached.erase(it);
			continue;
		}
		++it;
	}
}
void CCellBatcher::EndBatchFrame()
{
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

uint64_t CCellBatcher::ComputeGroupHash(const vector<OPAQUE_PACKET*>& packets) const
{
	uint64_t hashValue = 1469598103934665603ull;

	for (const OPAQUE_PACKET* packetPtr : packets)
	{
		if (!packetPtr || !packetPtr->pWorldMatrix)
			continue;

		const _float4x4& worldMatrix = *packetPtr->pWorldMatrix;

		// 모델/메쉬 식별
		hashValue = HashCombine64(hashValue, (uint64_t)(uintptr_t)packetPtr->pModel);
		hashValue = HashCombine64(hashValue, (uint64_t)packetPtr->DrawIndex);
		hashValue = HashCombine64(hashValue, (uint64_t)packetPtr->MaterialIndex);

		// 월드 위치(translation) 중심으로 넣기 (회전/스케일까지 포함하면 민감해져서 보통은 위치만)
		hashValue = HashCombine64(hashValue, HashFloatBits(worldMatrix._41));
		hashValue = HashCombine64(hashValue, HashFloatBits(worldMatrix._42));
		hashValue = HashCombine64(hashValue, HashFloatBits(worldMatrix._43));
	}

	// 개수도 반영
	hashValue = HashCombine64(hashValue, (uint64_t)packets.size());
	return hashValue;
}

CCellBatcher* CCellBatcher::Create(CRenderSystem* pRenderSys, const Options& opt)
{
	CCellBatcher* Instance = new CCellBatcher(pRenderSys,opt);
	return Instance;
}

void CCellBatcher::Free() {
	__super::Free();
	Clear();
	for (auto& cache : m_Cached)
	{
		cache.second.Release();
	}
}
