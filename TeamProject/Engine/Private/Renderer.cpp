#include "Engine_Defines.h"
#include "Renderer.h"

#include "Model.h"
#include "Shader.h"
#include "VIBuffer.h"
#include "RenderTarget.h"
#include "Target_Manager.h"
#include "PipeLine.h"
#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice{ pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}

CRenderer::~CRenderer()
{
}

HRESULT CRenderer::Initialize(CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	m_pPipeLine = pPipeLine;
	m_pTargetManager = pTargetManager;

	if(FAILED(CreateVIBuffer())) return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Create_RenderTarget(const RenderTargetDesc& desc)
{
	return 	m_pTargetManager->Create_Target(desc, false);
}

ID3D11ShaderResourceView* CRenderer::Get_CustomTargetSRV(const string strTag)
{
	CRenderTarget* pTarget = m_pTargetManager->Get_CustomTarget(strTag);
	if (!pTarget)
	{
		return nullptr;
	}
	return pTarget->Get_SRV();
}

ID3D11ShaderResourceView* CRenderer::Get_EngineTargetSRV(const string strTag)
{
	CRenderTarget* pTarget = m_pTargetManager->Get_EngineTarget(strTag);
	if (!pTarget)
	{
		return nullptr;
	}
	return pTarget->Get_SRV();
}

HRESULT CRenderer::LoadShader(string shaderName)
{
	m_pShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Shader(G_GlobalLevelKey, "Shader_Deferred.hlsl");
	if (nullptr == m_pShader)
		return E_FAIL;
	Safe_AddRef(m_pShader);
}

HRESULT CRenderer::CreateVIBuffer()
{
	m_pVIBuffer = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(G_GlobalLevelKey, "Engine_Default_Rect", BUFFER_TYPE::BASIC_RECT);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
	Safe_AddRef(m_pVIBuffer);
	return S_OK;
}

HRESULT CRenderer::Change_Viewport(_uint iWidth, _uint iHeight)
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iWidth;
	ViewPortDesc.Height = (_float)iHeight;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
	return S_OK;
}

HRESULT CRenderer::Get_InputLayout(CModel* pModel, CShader* pShader, _uint DrawIndex, const string& passConstant, ID3D11InputLayout** ppInputLayout)
{
	if (!pModel || !pShader || !ppInputLayout)
		return E_FAIL;

	string LayOutID = string(pModel->Get_ElementKey(DrawIndex)) + pShader->Get_Key();

	auto iter = m_InputLayouts.find(LayOutID);

	if (iter != m_InputLayouts.end())
	{
		*ppInputLayout = iter->second;
		return S_OK;
	}

	D3DX11_PASS_DESC passDesc = {};

	if (FAILED(pShader->GetPassSignature(passConstant, &passDesc))) return E_FAIL;

	if (pModel->Get_ElementCount(DrawIndex) == 0 || pModel->Get_ElementDesc(DrawIndex) == nullptr) return E_FAIL;

	HRESULT hr = m_pDevice->CreateInputLayout(
		pModel->Get_ElementDesc(DrawIndex), pModel->Get_ElementCount(DrawIndex),
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize,
		ppInputLayout);

	if (FAILED(hr)) return E_FAIL;

	m_InputLayouts.emplace(LayOutID, *ppInputLayout);

	return S_OK;
}

HRESULT CRenderer::Get_BufferInputLayout(CVIBuffer* pBuffer, CShader* pShader, const string& passConstant, ID3D11InputLayout** ppInputLayout)
{
	if (!pBuffer || !pShader || !ppInputLayout)
		return E_FAIL;

	string LayOutID = "Buffer_" + pBuffer->Get_Key() + "_" + pShader->Get_Key();

	auto iter = m_InputLayouts.find(LayOutID);

	if (iter != m_InputLayouts.end())
	{
		*ppInputLayout = iter->second;
		return S_OK;
	}

	D3DX11_PASS_DESC passDesc = {};

	if (FAILED(pShader->GetPassSignature(passConstant, &passDesc))) return E_FAIL;

	if (pBuffer->Get_ElementCount() == 0 || pBuffer->Get_ElementDesc() == nullptr) return E_FAIL;

	HRESULT hr = m_pDevice->CreateInputLayout(
		pBuffer->Get_ElementDesc(), pBuffer->Get_ElementCount(),
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize,
		ppInputLayout);

	if (FAILED(hr)) return E_FAIL;

	m_InputLayouts.emplace(LayOutID, *ppInputLayout);

	return S_OK;
}

void CRenderer::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pShader);
}
