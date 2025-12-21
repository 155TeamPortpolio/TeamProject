#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CStructuredBuffer :
    public CBase
{
public:
    typedef struct tagDesc
    {
        void* pInitData = nullptr;
        _uint iCount{};
        _uint iStride{};

        _bool UseSRV = true;
        _bool UseUAV = true;
        _uint iBindFlag{};
        _uint iUAVFlag{};

        D3D11_USAGE eUsage{};
        _uint iCpuAccess{};

        _bool UseStaging = false;
    }DESC;
private:
    CStructuredBuffer();
    virtual ~CStructuredBuffer() DEFAULT;

public:
    HRESULT Initialize(const DESC& desc);

    ID3D11Buffer* GetBuffer()const { return m_pBuffer; }
    ID3D11ShaderResourceView* GetSRV()const { return m_pSRV; }
    ID3D11UnorderedAccessView* GetUAV()const { return m_pUAV; }
    _uint GetCount()const { return m_iCount; }
    _uint GetStride()const { return m_iStride; }    
    _uint GetSize()const { return m_iCount * m_iStride; }

public:
    static CStructuredBuffer* Create(const DESC& desc);
    virtual void Free() override;

private:
    HRESULT CreateBuffer(const DESC& desc);
    HRESULT CreateSRV(const DESC& desc);
    HRESULT CreateUAV(const DESC& desc);
    HRESULT CreateStaging(const DESC& desc);

private:
    ID3D11Buffer* m_pBuffer = { nullptr };
    ID3D11ShaderResourceView* m_pSRV = { nullptr };
    ID3D11UnorderedAccessView* m_pUAV = { nullptr };
    ID3D11Buffer* m_pStaging = { nullptr };

    _uint m_iCount{};
    _uint m_iStride{};
};
NS_END
