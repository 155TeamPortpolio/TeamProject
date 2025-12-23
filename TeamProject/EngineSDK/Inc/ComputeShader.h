#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComputeShader final:
    public CBase
{
    enum CompileState { Cached, Compiled };
private:
    CComputeShader();
    virtual ~CComputeShader() DEFAULT;

public:
    HRESULT Initialize(ID3D11Device* pDevice, const string& filePath);

    void Bind(ID3D11DeviceContext* pDeviceContext);
    void Dispatch(ID3D11DeviceContext* pDeviceContext, _uint iGroupX, _uint iGroupY = 1, _uint iGroupZ = 1);
    void Dispatch1D(ID3D11DeviceContext* pDeviceContext, _uint iTotalThread, _uint iThreadGroupSize = 256);

    void SetCB(ID3D11DeviceContext* pDeviceContext, _uint slot, ID3D11Buffer* pCB);
    void SetSRV(ID3D11DeviceContext* pDeviceContext, _uint slot, ID3D11ShaderResourceView* pSRV);
    void SetUAV(ID3D11DeviceContext* pDeviceContext, _uint slot, ID3D11UnorderedAccessView* pUAV, _uint iInitialCount = 0xFFFFFFFF);

    static void UnbindAll(ID3D11DeviceContext* pDeviceContext, _uint iMaxSRV = 16, _uint iMaxUAV = 8);
public:
    static CComputeShader* Create(ID3D11Device* pDevice, const string& filePath, const string& shaderKey);
    void Free()override;

private:    
    CompileState Check_Chached(wstring wPath);

    HRESULT Compile_From_HLSL(ID3D11Device* pDevice, wstring wPath);
    HRESULT Compile_From_CSO(ID3D11Device* pDevice, wstring wPath);

    _uint m_ShaderID = {};
    string m_ShaderKey{};
    string m_EntryPoint{};
    wstring m_FileName = {};

    ID3D11ComputeShader* m_pCS = { nullptr };
    ID3DBlob* m_pByteCode = { nullptr };

    /* 컴퓨트 셰이더 기본 동작 - gpu의 스레드를 빌려서 연산에 사용
       
       CB(상수버퍼) -> dt, 옵션, 크기 등등 파라미터
       SRV -> 읽기 전용, Texture2D, StructuredBuffer 같은 입력,
       UAV -> 읽기, 쓰기 전용, RWStructuredBuffer, RWTexture2D 같은 출력 (파티클 데이터 갱신 등등)
    */
};

NS_END