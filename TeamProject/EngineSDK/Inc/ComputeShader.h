#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComputeShader :
    public CBase
{
private:
    CComputeShader();
    virtual ~CComputeShader() DEFAULT;

public:
    HRESULT Initialize(ID3D11Device* pDevice, const string& filePath, const string& shaderKey);

public:
    static CComputeShader* Create(ID3D11Device* pDevice, const string& filePath, const string& shaderKey);
    void Free()override;

private:
};

NS_END