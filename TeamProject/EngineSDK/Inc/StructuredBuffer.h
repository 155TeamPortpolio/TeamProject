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
        _uint iUAVFlag{};

        _bool UseStaging = false;
    }DESC;
private:
    CStructuredBuffer();
    virtual ~CStructuredBuffer() DEFAULT;

public:
    HRESULT Initialize(const DESC& desc);

    ID3D11ShaderResourceView* GetSRV()const { return m_pSRV; }
    ID3D11UnorderedAccessView* GetUAV()const { return m_pUAV; }

public:
    static CStructuredBuffer* Create(const DESC& desc);

private:
    HRESULT CreateBuffer(const DESC& desc);
    HRESULT CreateSRV();                  
    HRESULT CreateUAV(_uint iUAVFlag);
    HRESULT CreateStaging();

private:
    ID3D11Buffer* m_pBuffer = { nullptr };
    ID3D11ShaderResourceView* m_pSRV = { nullptr };
    ID3D11UnorderedAccessView* m_pUAV = { nullptr };
    ID3D11Buffer* m_pStaging = { nullptr };

    _uint m_iCount{};
    _uint m_iStride{};
};
NS_END
