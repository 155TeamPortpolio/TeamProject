#include "Engine_Defines.h"
#include "ComputeShader.h"
#include "Helper_Func.h"

CComputeShader::CComputeShader()
{
}

HRESULT CComputeShader::Initialize(ID3D11Device* pDevice, const string& filePath)
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

	return S_OK;
}

void CComputeShader::Bind(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->CSSetShader(m_pCS, nullptr, 0);
}

void CComputeShader::Dispatch(ID3D11DeviceContext* pDeviceContext, _uint iGroupX, _uint iGroupY, _uint iGroupZ)
{
	pDeviceContext->Dispatch(iGroupX, iGroupY, iGroupZ);
}

void CComputeShader::Dispatch1D(ID3D11DeviceContext* pDeviceContext, _uint iTotalThread, _uint iThreadGroupSize)
{
	_uint iGroupX = (iTotalThread + iThreadGroupSize - 1) / iThreadGroupSize;
	pDeviceContext->Dispatch(iGroupX, 1, 1);
}

void CComputeShader::SetCB(ID3D11DeviceContext* pDeviceContext, _uint slot, ID3D11Buffer* pCB)
{
	pDeviceContext->CSSetConstantBuffers(slot, 1, &pCB);
}

void CComputeShader::SetSRV(ID3D11DeviceContext* pDeviceContext, _uint slot, ID3D11ShaderResourceView* pSRV)
{
	pDeviceContext->CSSetShaderResources(slot, 1, &pSRV);
}

void CComputeShader::SetUAV(ID3D11DeviceContext* pDeviceContext, _uint slot, ID3D11UnorderedAccessView* pUAV, _uint iInitialCount)
{
	pDeviceContext->CSSetUnorderedAccessViews(slot, 1, &pUAV, &iInitialCount);
}

void CComputeShader::UnbindAll(ID3D11DeviceContext* pDeviceContext, _uint iMaxSRV, _uint iMaxUAV)
{
	vector<ID3D11ShaderResourceView*> nullSrvs(iMaxSRV, nullptr);
	vector<ID3D11UnorderedAccessView*> nullUavs(iMaxUAV, nullptr);
	vector<_uint> keep(iMaxUAV, 0xFFFFFFFF);

	pDeviceContext->CSSetShaderResources(0, iMaxSRV, nullSrvs.data());
	pDeviceContext->CSSetUnorderedAccessViews(0, iMaxUAV, nullUavs.data(), keep.data());
	pDeviceContext->CSSetShader(nullptr, nullptr, 0);
}

CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, const string& filePath, const string& shaderKey)
{
	CComputeShader* instance = new CComputeShader();
	if (FAILED(instance->Initialize(pDevice, filePath))) {
		MessageBoxA(nullptr, filePath.c_str(), "CComputeShader Create error", MB_OK);
		Safe_Release(instance);
	}
	if (instance)
		instance->m_ShaderKey = shaderKey;

	return instance;
}

void CComputeShader::Free()
{
	__super::Free();

	Safe_Release(m_pCS);
}

CComputeShader::CompileState CComputeShader::Check_Chached(wstring wPath)
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

HRESULT CComputeShader::Compile_From_HLSL(ID3D11Device* pDevice, wstring wPath)
{
	_uint		iCompileFlag = {};

#ifdef _DEBUG
	iCompileFlag = D3DCOMPILE_DEBUG
		| D3DCOMPILE_OPTIMIZATION_LEVEL1
		| D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#else
	iCompileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob* pErrorBlob = nullptr;

	HRESULT CompileHr = D3DCompileFromFile(
		wPath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"CS_MAIN",
		"cs_5_0",
		iCompileFlag,
		0,
		&m_pByteCode,
		&pErrorBlob
	);

	if (FAILED(CompileHr))
	{
		if (pErrorBlob)
			OutputDebugStringA(reinterpret_cast<_char*>(pErrorBlob->GetBufferPointer()));
		return E_FAIL;
	}

	HRESULT hr = pDevice->CreateComputeShader(
		m_pByteCode->GetBufferPointer(),
		m_pByteCode->GetBufferSize(),
		nullptr,
		&m_pCS
	);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CComputeShader::Compile_From_CSO(ID3D11Device* pDevice, wstring wPath)
{
	ID3DBlob* pEffectBlob = nullptr;

	filesystem::path csoPath = filesystem::path(wPath).parent_path() / filesystem::path(wPath).stem(); // ex) ../../Bin/VTX_Tile
	csoPath.replace_extension(L".cso");

	HRESULT hr = D3DReadFileToBlob(csoPath.wstring().c_str(), &pEffectBlob);
	if (FAILED(hr))
		return E_FAIL;

	m_pByteCode = pEffectBlob;

	hr = pDevice->CreateComputeShader(
		m_pByteCode->GetBufferPointer(),
		m_pByteCode->GetBufferSize(),
		nullptr,
		&m_pCS
	);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}
