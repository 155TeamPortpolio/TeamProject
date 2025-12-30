#include "Engine_Defines.h"
#include "StaticMeshRenderer.h"

#include "VIBuffer.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "Target_Manager.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Helper_Func.h"

CStaticMeshRenderer::CStaticMeshRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CRenderer(pDevice, pContext)
{
}

CStaticMeshRenderer::~CStaticMeshRenderer()
{
}

HRESULT CStaticMeshRenderer::Initialize(CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	__super::Initialize(pTargetManager, pPipeLine);

	LoadShader("Shader_Deferred.hlsl");
	Ready_Target();
	Ready_MRT();

	CreateSSAONoiseTexture();
	m_pPipeLine->Write_SSAOKernelBuffer(m_pDevice);
	return S_OK;
}

HRESULT CStaticMeshRenderer::Render_StaticMesh(StaticOpaquePass* pOpaquePass, InstancePass* pInstancePass)
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Deferred_Static"))) return E_FAIL;
	pOpaquePass->Execute(m_pContext, this);
	pInstancePass->Execute(m_pContext, this);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;

	return S_OK;
}

HRESULT CStaticMeshRenderer::Render_SSAO()
{
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_SSAO"))) return E_FAIL;

		m_pPipeLine->Update_SSAOBuffer(m_pContext);
		m_pTargetManager->Bind_Target("Target_Depth", m_pShader, "g_DepthTexture");
		m_pTargetManager->Bind_Target("Target_Normal", m_pShader, "g_NormalTexture");

		m_pShader->SetConstantBuffer("SSAOBuffer", m_pPipeLine->Get_SSAOBuffer());
		m_pShader->SetConstantBuffer("SSAOKernel", m_pPipeLine->Get_SSAOKernelBuffer());
		m_pShader->Bind_Value("g_SSAONoiseTexture", { m_pSSAONoiseTexture, "Texture2D", 0 });

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "SSAO", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("SSAO", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();
	}

	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_SSAO_Blur"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_SSAO", m_pShader, "g_SSAOTexture");
		m_pShader->SetConstantBuffer("SSAOBuffer", m_pPipeLine->Get_SSAOBuffer());

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "SSAO_BLUR", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("SSAO_BLUR", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();
	}
	return S_OK;
}

void CStaticMeshRenderer::Update(_float dt)
{
}

HRESULT CStaticMeshRenderer::Ready_Target()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	RenderTargetDesc EmissiveDesc = { "Target_Emissive" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(EmissiveDesc);

	RenderTargetDesc SSAODesc = { "Target_SSAO" , DXGI_FORMAT_R16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(SSAODesc);

	RenderTargetDesc SSAOBlurDesc = { "Target_SSAO_Blur" , DXGI_FORMAT_R16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(SSAOBlurDesc);

	return S_OK;
}

HRESULT CStaticMeshRenderer::Ready_MRT()
{
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Diffuse"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Normal"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Depth"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Metalic"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Emissive"))) return E_FAIL;

	if (FAILED(m_pTargetManager->Add_MRT("MRT_SSAO", "Target_SSAO"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_SSAO_Blur", "Target_SSAO_Blur"))) return E_FAIL;

	return S_OK;
}

HRESULT CStaticMeshRenderer::CreateSSAONoiseTexture()
{
	//SSAO NoiseTexture
	vector<_float4> ssaoNoise;

	for (unsigned int i = 0; i < 16; i++)
	{
		_float4 noise(Helper::Get_Random_Float(-1.f, 1.f), Helper::Get_Random_Float(-1.f, 1.f), 0.0f, 0.0f);
		ssaoNoise.push_back(noise);
	}

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = 4;
	texDesc.Height = 4;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = ssaoNoise.data();
	initData.SysMemPitch = 4 * sizeof(_float4);
	initData.SysMemSlicePitch = 0;


	ID3D11Texture2D* noiseTexture = nullptr;
	if (FAILED(m_pDevice->CreateTexture2D(&texDesc, &initData, &noiseTexture)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	if (FAILED(m_pDevice->CreateShaderResourceView(noiseTexture, &srvDesc, &m_pSSAONoiseTexture)))
	{
		Safe_Release(noiseTexture);
		return E_FAIL;
	}

	Safe_Release(noiseTexture);
	return S_OK;
}

CStaticMeshRenderer* CStaticMeshRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	CStaticMeshRenderer* Instance = new CStaticMeshRenderer(pDevice, pContext);
	if (FAILED(Instance->Initialize(pTargetManager, pPipeLine)))
	{
		Safe_Release(Instance);
	}
	return Instance;
}

void CStaticMeshRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pSSAONoiseTexture);
}
