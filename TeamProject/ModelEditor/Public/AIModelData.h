#pragma once
#include "ModelEditor_Defines.h"
#include "ModelData.h"

NS_BEGIN(ModelEdit)
class CAIModelData :
	public CModelData
{
	struct PosKey
	{
		int xi;
		int yi;
		int zi;

		bool operator==(const PosKey& other) const
		{
			return xi == other.xi && yi == other.yi && zi == other.zi;
		}
	};

	struct PosKeyHash
	{
		size_t operator()(const PosKey& key) const noexcept
		{
			size_t hashValue = 1469598103934665603ull;
			auto mix = [&](size_t value)
				{
					hashValue ^= value + 0x9e3779b97f4a7c15ull + (hashValue << 6) + (hashValue >> 2);
				};
			mix((size_t)key.xi);
			mix((size_t)key.yi);
			mix((size_t)key.zi);
			return hashValue;
		}
	};

private:
	CAIModelData();
	virtual ~CAIModelData() DEFAULT;

public:
	HRESULT Initialize(MESH_TYPE _eType, const aiScene* pAIScene);
	void Save_File(ofstream& ofs, _fmatrix PreTransform);

public:
	vector<_uint> Get_ProxyIndex() { return m_ProxyMarked; }
	vector<_uint> Get_LOD0_Index() { return m_LOD0Marked;  }
	vector<_uint> Get_LOD1_Index() { return m_LOD1Marked;  }
	vector<_uint> Get_LOD2_Index() { return m_LOD2Marked;  }
	vector<_uint> Get_LOD3_Index() { return m_LOD3Marked;  }
	vector<_uint> Get_Eff_Index()  { return m_EffMarked;   }
	vector<_uint> Get_MeshIndex_WithOutProxy();
	
private:
	vector<vector<_uint>> Find_Island(_uint numVertices, const vector<_uint>& indices);
	template<typename TVertex>
	vector<vector<_uint>> Find_Island_ByPosition(
		const vector<TVertex>& vertices,
		const vector<_uint>& indices,
		float epsilon);

	template <typename TVertex>
	static void Build_IslandMesh(
		_uint srcVertexCount,
		const vector<TVertex>& srcVertices,
		const vector<_uint>& srcIndices,
		const vector<_uint>& islandFaces,
		vector<TVertex>& dstVertices,
		vector<_uint>& dstIndices);
private:
	vector<_uint> m_NotProxy;
	vector<_uint> m_ProxyMarked;
	vector<_uint> m_LOD0Marked;
	vector<_uint> m_LOD1Marked;
	vector<_uint> m_LOD2Marked;
	vector<_uint> m_LOD3Marked;
	vector<_uint> m_EffMarked;
public:
	static CModelData* Create(MESH_TYPE _eType, const aiScene* pAIScene);
	virtual void Free() override;
};

NS_END



template<typename TVertex>
inline void CAIModelData::Build_IslandMesh(
	_uint srcVertexCount, const vector<TVertex>& srcVertices, const vector<_uint>& srcIndices,
	const vector<_uint>& islandFaces, vector<TVertex>& dstVertices, vector<_uint>& dstIndices)
{
	dstVertices.clear();
	dstIndices.clear();
	dstIndices.reserve(islandFaces.size() * 3);

	//원본 정점 만큼 배열을 키워둠. (-1이면 아직 복사 안된 거)
	vector<_int> remap(srcVertexCount, -1);

	/*원본 메쉬에서 정점이 섬에서는 몇번쨰 정점인지 변환해주는 람다임*/
	auto MapIndex = [&](_uint oldIndex) -> _uint
		{
			_int& mapped = remap[oldIndex];
			if (mapped < 0) /*아직 복사 안된 정점이라면*/
			{
				mapped = (_int)dstVertices.size(); /*섬 정점 인덱스에 추가*/
				dstVertices.push_back(srcVertices[oldIndex]);  //이점 정점의 정보(구조체)넣어줌
			}
			return (_uint)mapped;
		};

	for (_uint faceNum : islandFaces)
	{
		_uint old0 = srcIndices[faceNum * 3 + 0];
		_uint old1 = srcIndices[faceNum * 3 + 1];
		_uint old2 = srcIndices[faceNum * 3 + 2];

		dstIndices.push_back(MapIndex(old0));
		dstIndices.push_back(MapIndex(old1));
		dstIndices.push_back(MapIndex(old2));
	}
}
template<typename TVertex>
inline vector<vector<_uint>> CAIModelData::Find_Island_ByPosition(const vector<TVertex>& vertices, const vector<_uint>& indices, float epsilon)
{

	vector<vector<_uint>> islands;
	if (indices.empty() || indices.size() % 3 != 0 || vertices.empty()) return islands;

	const _uint vertexCount = (_uint)vertices.size();
	const _uint faceCount = (_uint)(indices.size() / 3);

	// 1) vertex -> groupId
	unordered_map<PosKey, _uint, PosKeyHash> keyToGroup;
	keyToGroup.reserve(vertexCount);

	vector<_uint> vertexToGroup(vertexCount, 0);
	_uint groupCount = 0;

	auto makeKey = [&](const _float3& p)->PosKey {
		const float inv = 1.0f / epsilon;
		return PosKey{
			(int)std::floor(p.x * inv + 0.5f),
			(int)std::floor(p.y * inv + 0.5f),
			(int)std::floor(p.z * inv + 0.5f)
		};
		};

	for (_uint i = 0; i < vertexCount; ++i) {
		PosKey key = makeKey(vertices[i].vPosition);
		auto it = keyToGroup.find(key);
		if (it == keyToGroup.end()) {
			keyToGroup.emplace(key, groupCount);
			vertexToGroup[i] = groupCount;
			++groupCount;
		}
		else {
			vertexToGroup[i] = it->second;
		}
	}

	// 2) group -> faces
	vector<vector<_uint>> groupToFace(groupCount);
	for (_uint face = 0; face < faceCount; ++face) {
		_uint a = indices[face * 3 + 0], b = indices[face * 3 + 1], c = indices[face * 3 + 2];
		if (a < vertexCount) groupToFace[vertexToGroup[a]].push_back(face);
		if (b < vertexCount) groupToFace[vertexToGroup[b]].push_back(face);
		if (c < vertexCount) groupToFace[vertexToGroup[c]].push_back(face);
	}

	// 3) BFS on faces
	vector<uint8_t> visited(faceCount, 0);
	vector<_uint> stack;
	islands.reserve(8);

	for (_uint start = 0; start < faceCount; ++start) {
		if (visited[start]) continue;
		visited[start] = 1;
		islands.emplace_back();
		auto& comp = islands.back();
		stack.clear();
		stack.push_back(start);

		while (!stack.empty()) {
			_uint f = stack.back(); stack.pop_back();
			comp.push_back(f);

			_uint a = indices[f * 3 + 0], b = indices[f * 3 + 1], c = indices[f * 3 + 2];
			auto push = [&](_uint vi) {
				if (vi >= vertexCount) return;
				_uint g = vertexToGroup[vi];
				for (_uint nb : groupToFace[g]) {
					if (!visited[nb]) {
						visited[nb] = 1;
						stack.push_back(nb);
					}
				}
				};
			push(a); push(b); push(c);
		}
	}
	return islands;
}