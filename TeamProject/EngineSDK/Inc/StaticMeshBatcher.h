#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CStaticMeshBatcher 
    : public CBase
{   
    struct SubmitDesc
    {
        class CMesh* mesh = { nullptr };
        class CMaterialInstance* material = { nullptr };
        _float4x4 world;
    };
    struct BatchKey
    {
        class CModel* pModel = { nullptr };
        class CMaterial* pMaterial = { nullptr };
        _uint drawIndex = 0;
        _uint materialIndex = 0;
        string passConstant;

        _bool operator==(const BatchKey& rhs) const
        {
            return pModel == rhs.pModel &&
                   pMaterial == rhs.pMaterial &&
                   drawIndex == rhs.drawIndex &&
                   materialIndex == rhs.materialIndex &&
                   passConstant == rhs.passConstant;
        }
    };

    struct BatchKeyHasher
    {
        size_t operator()(const BatchKey& key) const noexcept
        {
            size_t hashValue = 0;
            auto mix = [&](size_t value)
                {
                    hashValue ^= value + 0x9e3779b97f4a7c15ull + (hashValue << 6) + (hashValue >> 2);
                };

            mix(reinterpret_cast<size_t>(key.pModel));
            mix(reinterpret_cast<size_t>(key.pMaterial));
            mix(static_cast<size_t>(key.drawIndex));
            mix(static_cast<size_t>(key.materialIndex));
            mix(hash<std::string>()(key.passConstant));
            return hashValue;
        }
    };

    struct BatchGroup
    {
        BatchKey key{};
        vector<_uint> transformIndices;
    };

private:
    CStaticMeshBatcher();
    ~CStaticMeshBatcher() DEFAULT;
public:
    HRESULT Initialize(ID3D11Device* device, _uint maxInstancesPerBatch = 4096);
    void Submit(const OPAQUE_PACKET& packet);

    void BeginFrame();
    void Flush(ID3D11DeviceContext* context, class RenderPass* pRenderPass,class CRenderer* pRenderer);

private:
    void EnsureInstanceIndexBuffer();
    void UploadInstanceIndices(ID3D11DeviceContext* context, const uint32_t* indices, uint32_t count);

private:
    ID3D11Device* m_pDevice = { nullptr };
    _uint m_maxInstances = 4096;
    unordered_map<BatchKey, BatchGroup, BatchKeyHasher> m_Batchgroups;

    // ¹èÄª¿ë: StructuredBuffer<uint>
    ID3D11Buffer* m_instanceIndexBuffer = { nullptr };
    ID3D11ShaderResourceView* m_instanceIndexSRV = { nullptr };

public:
    static CStaticMeshBatcher* Create(ID3D11Device* device, _uint maxInstancesPerBatch);
    virtual void Free() override;
};

NS_END