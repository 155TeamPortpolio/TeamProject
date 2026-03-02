#pragma once
#include "Base.h"
static constexpr _uint BonePages = { 10 };

NS_BEGIN(Engine)
class CSkinningBuffer :
    public CBase
{
    struct Allocation
    {
        _uint boneOffset = 0; 
        _uint boneCount = 0;
        _uint pageIndex = 0;  
    };

    struct SkinningPage {
        ID3D11Buffer* pBuffer = { nullptr };
        ID3D11ShaderResourceView* pShaderResourceView = {nullptr};
        _uint Capacity = {};
        _uint CursorBones = {};
        _bool begunThisFrame = { false };

        void Release() {
            Safe_Release(pBuffer);
            Safe_Release(pShaderResourceView);
        }
    };

private:
    CSkinningBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CSkinningBuffer() DEFAULT;

public:
    HRESULT Initialize();
    void BeginFrame(); 
private:
    _bool Upload(const void* matrixFloat4x4Array, _uint boneCount, Allocation& outAlloc);
    void BindSRV(ID3D11DeviceContext* context, _uint slot) const;
    ID3D11ShaderResourceView* GetSRV(_uint pageIndex) const;

private:
    _bool CreatePages();
    _bool EnsurePageBegun(ID3D11DeviceContext* context, SkinningPage& page, bool discard);
    _bool WriteToPage(ID3D11DeviceContext* context, SkinningPage& page, const void* matrixFloat4x4Array, _uint boneCount, _uint& outOffset);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
private:
    vector<SkinningPage> m_Pages;
    _uint m_CurIndex = {};
    _uint m_MaxBonePerPage = {};

public:
    static CSkinningBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};

NS_END