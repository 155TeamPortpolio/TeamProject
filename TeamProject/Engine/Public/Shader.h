#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class ENGINE_DLL CShader final:
    public CBase
{
    enum CompileState{Cached,Compiled};

public:
    typedef struct ShaderVariableTypeDesc {
        string   parentCBufferName = {};
        string typeName = {};
        string constantName = {};
        _uint varSize = {};
        ID3DX11EffectVariable* pHandle = {nullptr};
    }SHADER_VAR_DESC;

    typedef struct ShaderConstantBufferDesc {
        string Name;
        uint32_t RegisterSlot;
        uint32_t Size;
        ID3DX11EffectConstantBuffer* pHandle = { nullptr };
    } CBUFFER_DESC;

private:
    CShader();
    virtual ~CShader() override;

public:
    HRESULT Initialize(ID3D11Device* pDevice,  const string& filePath);
  /*Render*/
    void Apply(const string& m_passConstant,ID3D11DeviceContext* pContext);
    HRESULT Reset_Value();
    HRESULT Bind_Value(const string& ConstantName, const SHADER_PARAM& parameter);
    HRESULT SetConstantBuffer(const string& ConstantName, ID3D11Buffer* pData);

public:
    const string& Get_Key() { return m_ShaderKey; }
    _uint Get_ID() { return m_ShaderID; }
    void Set_Key(const string& _shaderKey) { m_ShaderKey = _shaderKey; }
    vector<string>  Get_PassList();
    HRESULT GetPassSignature(UINT iPassIndex, D3DX11_PASS_DESC* pOutPassDesc);
    HRESULT GetPassSignature(const string& m_passConstant, D3DX11_PASS_DESC* pOutPassDesc);

private:
    HRESULT Bind_Matrix(const string& ConstantName, const _float4x4* pMatrix);
    HRESULT Bind_ShaderResource(const string& ConstantName, ID3D11ShaderResourceView* pSRV);
    HRESULT Bind_ShaderResourceArray(const string& ConstantName, ID3D11ShaderResourceView* pSRVArr);

private: //Compile
    void ReflectShader();
    HRESULT InitializeTechnique(wstring wPath);
    CompileState Check_Chached(wstring wPath);

    HRESULT Compile_From_HLSL(ID3D11Device* pDevice, wstring wPath);
    HRESULT Compile_From_CSO(ID3D11Device* pDevice, wstring wPath);

private:
    _uint m_ShaderID = {};
    string m_ShaderKey;
    wstring m_FileName = {};

    ID3DX11Effect* m_pEffect = { nullptr };
    ID3DX11EffectTechnique* m_pTechnique = { nullptr };

    unordered_map<string, ID3DX11EffectPass*> m_Passes;         /*패스들 미리 저장*/
    unordered_map<string, SHADER_VAR_DESC> m_Variables;     /*변수들 미리 저장*/ //근데 변수 안에 상수 버퍼 ㅂ변수들이 들어가. 근데 내가 아래에 저장하는거는 상수버퍼의 핸들임
    unordered_map<string, CBUFFER_DESC> m_CBuffers;             /*상수버퍼 미리 저장*/

public:
   static CShader* Create(ID3D11Device* pDevice, const string& filePath, const string& shaderKey);
   virtual void Free() override;
   static _uint s_NextID;
};

NS_END