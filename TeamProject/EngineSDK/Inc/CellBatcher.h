#pragma once
#include "Base.h"
#include "StaticBathcer.h"
#include "RenderPass.h"
#include "Renderer.h"

NS_BEGIN(Engine)

class CCellBatcher :
    public CBase
{
public:
    struct Options
    {
        _float cellSize = 30.f;       // 너 월드 스케일에 맞게
        _uint minBatchCount = 6;     // 이 이상일 때만 배칭
        _uint maxBuildPerFrame = 2;  // 프레임 hitch 방지
        _uint keepCachedFrames = 120;  // 프레임 hitch 방지
    };
    struct CellRange
    {
        int minX, maxX;
        int minZ, maxZ;
    };

private:
    explicit CCellBatcher(class CRenderSystem* pRenderSys, const Options& opt);
    ~CCellBatcher() DEFAULT;

public:
    void BeginBatchFrame(_uint frameIndex);
    void SubmitVisiblePacket(OPAQUE_PACKET& packet);
    void BuildBatchesIfNeeded(ID3D11Device* device);
    _uint DrawBatches(ID3D11DeviceContext* context, RenderPass* pass, CRenderer* renderer);
    void TrimCache();
    void EndBatchFrame();
    void Clear(); // 캐시 전부 제거

private:
    void AppendWorldBaked(
        const vector<VTXMESH>& srcVertices,   // 원본 메쉬의 로컬 정점
        const vector<_uint>& srcIndices,       // 원본 인덱스
        const _float4x4& world,                // 이 오브젝트의 월드 행렬
        vector<VTXMESH>& outVertices,          // 누적될 버텍스 버퍼
        vector<_uint>& outIndices              // 누적될 인덱스 버퍼
    );

    ID3D11InputLayout* GetOrCreateBatchInputLayout(ID3D11Device* device, CShader* shader, const char* passName);
    uint64_t ComputeGroupHash(const vector<OPAQUE_PACKET*>& packets) const;


private:
    CellKey MakeCellKey(const _float4x4& world,class CModel* pModel, _uint drawIndex) const;
    bool CanBatch(const OPAQUE_PACKET& packet) const;
    bool BuildOneBatch(ID3D11Device* device, const CellBatchKey& key, const vector<OPAQUE_PACKET*>& packets);
    CellRange MakeCellRange(const _float4x4& worldMatrix, CModel* pModel, _uint drawIndex) const;
    

private:
    Options m_Options;
    _uint m_iFrameIndex = 0;
    class CRenderSystem* m_pRenderSystem = { nullptr };
    unordered_map<CellBatchKey, vector<OPAQUE_PACKET*>, CellBatchKeyHash> m_BatchGroups;
    unordered_map<CellBatchKey, CachedBatch, CellBatchKeyHash> m_Cached;
    unordered_map<string, ID3D11InputLayout*> m_LayoutCache;
public:
    static CCellBatcher* Create(class CRenderSystem* pRenderSys,const Options& opt = {});
    virtual void Free();
};

NS_END