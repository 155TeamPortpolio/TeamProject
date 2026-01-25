#include "Engine_Defines.h"
#include "Shader.h"
#include "Helper_Func.h"
#include "GUIUtil.h"
#include "Texture.h"

_uint CShader::s_NextID = 1;

CShader::CShader()
	:m_ShaderID(s_NextID++)
{
}

CShader::~CShader()
{
}

HRESULT CShader::Initialize(ID3D11Device* pDevice, const string& filePath)
{
	wstring wPath = Helper::ConvertToWideString(filePath);

	CompileState eState = Check_Chached(wPath);

	if (eState == Cached) {
		if (FAILED(Compile_From_CSO(pDevice, wPath))) {
			return E_FAIL;
		}
	}
	else {
		if (FAILED(Compile_From_HLSL(pDevice, wPath))) {
			return E_FAIL;
		}
	}


	return InitializeTechnique(wPath);
}

HRESULT CShader::GetPassSignature(UINT iPassIndex, D3DX11_PASS_DESC* pOutPassDesc)
{
	if (!m_pTechnique || !pOutPassDesc)
		return E_FAIL;

	ID3DX11EffectPass* pPass = m_pTechnique->GetPassByIndex(iPassIndex);

	if (!pPass)
		return E_FAIL;

	pPass->GetDesc(pOutPassDesc);
	return S_OK;
}

HRESULT CShader::GetPassSignature(const string& m_passConstant, D3DX11_PASS_DESC* pOutPassDesc)
{
	auto iter = m_Passes.find(m_passConstant);
	if (iter != m_Passes.end()) {
		iter->second->GetDesc(pOutPassDesc);
		return S_OK;
	}
	return E_FAIL;
}


void CShader::Apply(const string& m_passConstant, ID3D11DeviceContext* pContext)
{
	auto iter = m_Passes.find(m_passConstant);
	if (iter != m_Passes.end()) {
		HRESULT hr = iter->second->Apply(0, pContext);
	}
}

HRESULT CShader::Bind_Value(const string& ConstantName, const SHADER_PARAM& parameter)
{
	if (parameter.typeName == "cbuffer") {
		auto iter = m_CBuffers.find(ConstantName);

		if (iter == m_CBuffers.end())
			return E_FAIL;

		HRESULT hr = iter->second.pHandle->SetRawValue(parameter.pData, 0, parameter.iSize);

		if (FAILED(hr)) {
			return E_FAIL;
		}
		return hr;
	}
	else {
		auto iter = m_Variables.find(ConstantName);

		if (iter == m_Variables.end())
			return E_FAIL;


		if (parameter.typeName == "float4x4")
			return Bind_Matrix(ConstantName, static_cast<const _float4x4*>(parameter.pData));
		else if (parameter.typeName == "Texture2D")
			return Bind_ShaderResource(ConstantName, static_cast<ID3D11ShaderResourceView*>(parameter.pData));
		else if (parameter.typeName == "Texture2DArray")
			return Bind_ShaderResourceArray(ConstantName, static_cast<ID3D11ShaderResourceView*>(parameter.pData));
		else if (parameter.typeName == "StructuredBuffer")
			return Bind_ShaderResource(ConstantName, static_cast<ID3D11ShaderResourceView*>(parameter.pData));
		else if (parameter.typeName == "float4x4[]") {
			return Bind_MatrixArray(ConstantName, reinterpret_cast<const _float4x4*>(parameter.pData), parameter.iSize / sizeof(_float4x4)  // 배열 크기 계산
			);
		}
		HRESULT hr = SetRawValueOrClear(iter->second.pHandle, parameter.pData, parameter.iSize);
		if (FAILED(hr)) {
			return E_FAIL;
		}
		return hr;

	}
}

HRESULT CShader::SetRawValueOrClear(ID3DX11EffectVariable* effectVariable, const void* dataPtr, UINT byteSize)
{
	if (!effectVariable || !effectVariable->IsValid())
		return E_FAIL;

	if (dataPtr && byteSize > 0)
		return effectVariable->SetRawValue(dataPtr, 0, byteSize);

	UINT resolvedSize = byteSize;
	if (resolvedSize == 0)
	{
		ID3DX11EffectType* effectType = effectVariable->GetType();
		if (!effectType || !effectType->IsValid())
			return E_FAIL;

		D3DX11_EFFECT_TYPE_DESC typeDesc = {};
		HRESULT hr = effectType->GetDesc(&typeDesc);
		if (FAILED(hr) || typeDesc.PackedSize == 0)
			return E_FAIL;

		resolvedSize = typeDesc.PackedSize;
	}

	// 0 버퍼 만들어서 덮어쓰기
	vector<unsigned char> zeroBuffer(resolvedSize, 0);
	return effectVariable->SetRawValue(zeroBuffer.data(), 0, resolvedSize);
}

HRESULT CShader::SetConstantBuffer(const string& ConstantName, ID3D11Buffer* pData)
{
	auto iter = m_CBuffers.find(ConstantName);
	if (iter == m_CBuffers.end()) {
		MSG_BOX("Wrong ConstantBuffer is Binding : CShader");
		return E_FAIL;
	}

	return iter->second.pHandle->SetConstantBuffer(pData);
}

vector<string>  CShader::Get_PassList()
{
	vector<string> passConstant;

	for (auto& pass : m_Passes)
		passConstant.push_back(pass.first);

	return passConstant;
}

HRESULT CShader::Bind_Matrix(const string& ConstantName, const _float4x4* pMatrix)
{
	auto iter = m_Variables.find(ConstantName);
	if (iter == m_Variables.end()) {
		MSG_BOX("Wrong Variable Name is Binding : CShader");
		return E_FAIL;
	}

	ID3DX11EffectMatrixVariable* pMatrixVariable = iter->second.pHandle->AsMatrix();
	if (!pMatrixVariable) {
		MSG_BOX("Wrong Variable Type is Binding : CShader");
		return E_FAIL;
	}

	pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
	return S_OK;
}

HRESULT CShader::Bind_ShaderResource(const string& ConstantName, ID3D11ShaderResourceView* pSRV)
{
	auto iter = m_Variables.find(ConstantName);
	if (iter == m_Variables.end()) {
		MSG_BOX("Wrong Variable Name is Binding : CShader");
		return E_FAIL;
	}

	ID3DX11EffectShaderResourceVariable* pShaderVariable = iter->second.pHandle->AsShaderResource();
	if (!pShaderVariable || !pShaderVariable->IsValid()) {
		MSG_BOX("Wrong Variable Type is Binding : CShader");
		return E_FAIL;
	}

	pShaderVariable->SetResource(pSRV);
	return S_OK;
}

HRESULT CShader::Bind_ShaderResourceArray(const string& ConstantName, ID3D11ShaderResourceView* pSRVArr)
{
	if (!pSRVArr || ConstantName.empty())
		return S_OK;

	auto iter = m_Variables.find(ConstantName);
	if (iter == m_Variables.end()) {
		MSG_BOX("Invalid variable name :CShader ");
		return E_FAIL;
	}

	ID3DX11EffectShaderResourceVariable* pShaderVariable = iter->second.pHandle->AsShaderResource();
	if (!pShaderVariable) {
		MSG_BOX("Invalid variable type :CShader ");
		return E_FAIL;
	}

	HRESULT hr = pShaderVariable->SetResource(pSRVArr);
	if (FAILED(hr)) {
		MSG_BOX("Failed to bind Shader Resource : CShader");
		return hr;
	}
	return S_OK;
}

HRESULT CShader::Bind_MatrixArray(const string& ConstantName, const _float4x4* pMatrices, _uint iCount)
{
	auto iter = m_Variables.find(ConstantName);
	if (iter == m_Variables.end()) {
		MSG_BOX("Wrong Variable Name is Binding : CShader");
		return E_FAIL;
	}

	ID3DX11EffectMatrixVariable* pMatrixVariable = iter->second.pHandle->AsMatrix();
	if (!pMatrixVariable || !pMatrixVariable->IsValid()) {
		MSG_BOX("Wrong Variable Type is Binding : CShader");
		return E_FAIL;
	}

	HRESULT hr = pMatrixVariable->SetMatrixArray(
		reinterpret_cast<const float*>(pMatrices),
		0,
		iCount
	);

	if (FAILED(hr)) {
		MSG_BOX("Failed to bind Matrix Array : CShader");
		return hr;
	}

	return S_OK;
}

void CShader::CacheDefaults()
{
	m_Defaults.clear();
	for (auto& pair : m_Variables)
	{
		const std::string& name = pair.first;
		ID3DX11EffectVariable* var = pair.second.pHandle;
		if (!var || !var->IsValid())
			continue;

		ID3DX11EffectType* type = var->GetType();
		if (!type || !type->IsValid())
			continue;

		D3DX11_EFFECT_TYPE_DESC typeDesc = {};
		if (FAILED(type->GetDesc(&typeDesc)))
			continue;

		if (typeDesc.Class == D3D_SVC_OBJECT &&
			(typeDesc.Type == D3D_SVT_TEXTURE2D || typeDesc.Type == D3D_SVT_TEXTURE2DARRAY ||
				typeDesc.Type == D3D_SVT_BUFFER || typeDesc.Type == D3D_SVT_STRUCTURED_BUFFER))
		{
			CachedDefault def;
			def.kind = CachedDefault::Kind::SRV;
			def.pSRV = nullptr; 
			m_Defaults.emplace(name, std::move(def));
			continue;
		}

		const _uint packed = typeDesc.PackedSize;
		if (packed > 0)
		{
			CachedDefault def;
			def.kind = CachedDefault::Kind::Raw;
			def.rawSize = packed;
			def.raw.resize(packed);

			if (SUCCEEDED(var->GetRawValue(def.raw.data(), 0, packed)))
				m_Defaults.emplace(name, std::move(def));
		}
	}
}

void CShader::ReflectShader()
{
	D3DX11_EFFECT_DESC effectDesc;
	m_pEffect->GetDesc(&effectDesc);

	for (size_t i = 0; i < effectDesc.GlobalVariables; i++)
	{
		ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByIndex(i);
		if (!pVariable->IsValid())
			continue;

		D3DX11_EFFECT_VARIABLE_DESC varDesc;
		pVariable->GetDesc(&varDesc);

		ID3DX11EffectType* pType = pVariable->GetType();
		D3DX11_EFFECT_TYPE_DESC tType;
		pType->GetDesc(&tType);

		string typname = tType.TypeName;
		SHADER_VAR_DESC variableSlot{};
		variableSlot.pHandle = pVariable;
		variableSlot.constantName = varDesc.Name;
		variableSlot.typeName = tType.TypeName;

		ID3DX11EffectConstantBuffer* pParentCBuffer = pVariable->GetParentConstantBuffer();

		if (pParentCBuffer->IsValid())
		{
			D3DX11_EFFECT_VARIABLE_DESC cbVarDesc;
			pParentCBuffer->GetDesc(&cbVarDesc);
			variableSlot.parentCBufferName = cbVarDesc.Name;
		}
		m_Variables.emplace(varDesc.Name, variableSlot);
	}

	for (size_t i = 0; i < effectDesc.ConstantBuffers; i++)
	{
		ID3DX11EffectConstantBuffer* pConstant = m_pEffect->GetConstantBufferByIndex(i);
		if (!pConstant->IsValid())
			continue;

		D3DX11_EFFECT_VARIABLE_DESC varDesc;
		pConstant->GetDesc(&varDesc);

		ID3DX11EffectType* pType = pConstant->GetType();
		D3DX11_EFFECT_TYPE_DESC typeDesc;
		pType->GetDesc(&typeDesc);

		CBUFFER_DESC cbDesc = {};
		cbDesc.Name = varDesc.Name;
		cbDesc.pHandle = pConstant;
		cbDesc.Size = typeDesc.UnpackedSize;

		m_CBuffers.emplace(cbDesc.Name, cbDesc);
	}
}
void CShader::ResetToDefaults()
{
	for (auto& pair : m_Defaults)
	{
		const string& name = pair.first;
		const CachedDefault& def = pair.second;

		auto it = m_Variables.find(name);
		if (it == m_Variables.end())
			continue;

		if (it->second.parentCBufferName.empty())
			continue;
		if (it->second.parentCBufferName != "$Globals")
			continue;

		ID3DX11EffectVariable* var = it->second.pHandle;
		if (!var || !var->IsValid())
			continue;

		if (def.kind == CachedDefault::Kind::SRV)
		{
			auto* srvVar = var->AsShaderResource();
			if (srvVar && srvVar->IsValid())
				srvVar->SetResource(def.pSRV); 
		}
		else
		{
			if (!def.raw.empty())
				var->SetRawValue(def.raw.data(), 0, def.rawSize);
		}
	}
}

HRESULT CShader::InitializeTechnique(wstring wPath)
{
	m_Passes.clear();
	m_Variables.clear();
	m_CBuffers.clear();
	m_Defaults.clear();
	Safe_Release(m_pTechnique);

	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	D3DX11_TECHNIQUE_DESC tDesc{};
	m_pTechnique->GetDesc(&tDesc);

	if (nullptr == m_pTechnique)
		return E_FAIL;

	for (size_t i = 0; i < tDesc.Passes; i++)
	{
		D3DX11_PASS_DESC desc{};
		m_pTechnique->GetPassByIndex(i)->GetDesc(&desc);
		m_Passes.emplace(desc.Name, m_pTechnique->GetPassByIndex(i));
	}

	m_FileName = filesystem::path(wPath).stem().wstring();
	ReflectShader();
	CacheDefaults();
	return S_OK;
}

CShader::CompileState CShader::Check_Chached(wstring wPath)
{
	filesystem::path HLSLpath = filesystem::path(wPath);

	filesystem::path csoPath = HLSLpath.parent_path() / HLSLpath.stem(); // ex) ../../Bin/VTX_Tile
	csoPath.replace_extension(L".cso");

	if (!filesystem::exists(csoPath)) {
		return CompileState::Compiled;
	}
	//if (filesystem::last_write_time(filesystem::path(HLSLpath)) <= filesystem::last_write_time(filesystem::path(csoPath))) {
	//	return CompileState::Compiled;
	//};

	return CompileState::Cached;
}

HRESULT CShader::Compile_From_HLSL(ID3D11Device* pDevice, wstring wPath)
{
	Safe_Release(m_pTechnique);
	Safe_Release(m_pEffect);
	ClearEffectCaches();

	_uint		iCompileFlag = {};

#ifdef _DEBUG
	iCompileFlag = D3DCOMPILE_DEBUG
		| D3DCOMPILE_OPTIMIZATION_LEVEL1
		| D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#else
	iCompileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob* pErrorBlob = nullptr;

	HRESULT CompileHr = D3DX11CompileEffectFromFile(
		wPath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		iCompileFlag,
		0,
		pDevice,
		&m_pEffect,
		&pErrorBlob
	);

	if (FAILED(CompileHr)) {
		if (pErrorBlob) {
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader::Compile_From_CSO(ID3D11Device* pDevice, wstring wPath)
{
	ID3DBlob* pEffectBlob = nullptr;

	filesystem::path csoPath = filesystem::path(wPath).parent_path() / filesystem::path(wPath).stem(); // ex) ../../Bin/VTX_Tile
	csoPath.replace_extension(L".cso");

	HRESULT hr = D3DReadFileToBlob(csoPath.wstring().c_str(), &pEffectBlob);
	if (SUCCEEDED(hr))
	{
		ID3DX11Effect* pEffect = nullptr;
		hr = D3DX11CreateEffectFromMemory(
			pEffectBlob->GetBufferPointer(),
			pEffectBlob->GetBufferSize(),
			0,
			pDevice,
			&pEffect
		);

		if (SUCCEEDED(hr))
			m_pEffect = pEffect;
		return S_OK;
	}
	return E_FAIL;
}

void CShader::ClearEffectCaches()
{
	m_Passes.clear();
	m_Variables.clear();
	m_CBuffers.clear();
	m_Defaults.clear();
	m_pTechnique = nullptr; // 기존 포인터 무효
}

CShader* CShader::Create(ID3D11Device* pDevice, const string& filePath, const string& shaderKey)
{
	CShader* instance = new CShader();
	if (FAILED(instance->Initialize(pDevice, filePath))) {
		MessageBoxA(nullptr, filePath.c_str(), "CShader Create error", MB_OK);
		Safe_Release(instance);
	}
	if (instance)
		instance->m_ShaderKey = shaderKey;

	return instance;
}

void CShader::Free()
{
	__super::Free();
	Safe_Release(m_pTechnique);
	Safe_Release(m_pEffect);

	m_Passes.clear();
}
