#include "Engine_Defines.h"
#include "StaticMeshRenderer.h"

#include "VIBuffer.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "Target_Manager.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
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

	LoadShader("Shader_Deferred_StaticMesh.hlsl");
	Ready_Target();
	Ready_MRT();

	CreateSSAONoiseTexture();
	m_pPipeLine->Write_SSAOKernelBuffer(m_pDevice);
	return S_OK;
}

HRESULT CStaticMeshRenderer::Render_StaticMesh(StaticOpaquePass* pOpaquePass, InstancePass* pInstancePass)
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred_Skinned");

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Deferred_Static",  0xFF, pDeferredDSV, false))) return E_FAIL;
	pOpaquePass->Execute(m_pContext, this);
	pInstancePass->Execute(m_pContext, this);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;

	return S_OK;
}

HRESULT CStaticMeshRenderer::Render_StaticMesh_Bloom()
{
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_Bloom_Static_H"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_Emissive", m_pShader, "MeshBrightTexture");
		m_pTargetManager->Bind_Target("Target_EmissiveInfo", m_pShader, "MeshBloomInfo");
		Bind_WorldMatrix();

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "BLOOM_BLURX", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("BLOOM_BLURX", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();
	}

	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_Bloom_Static_V"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_BloomBlurX_StaticMesh", m_pShader, "MeshBlurXTexture");
		m_pTargetManager->Bind_Target("Target_EmissiveInfo", m_pShader, "MeshBloomInfo");
		Bind_WorldMatrix();

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "BLOOM_BLURY", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("BLOOM_BLURY", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();
	}
	return S_OK;
}

HRESULT CStaticMeshRenderer::Render_SSAO()
{
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_SSAO"))) return E_FAIL;

		m_pPipeLine->Update_SSAOBuffer(m_pContext);
		m_pTargetManager->Bind_Target("Target_Static_Depth", m_pShader, "DepthTexture");
		m_pTargetManager->Bind_Target("Target_Static_Normal", m_pShader, "NormalTexture");

		m_pShader->SetConstantBuffer("SSAOBuffer", m_pPipeLine->Get_SSAOBuffer());
		m_pShader->SetConstantBuffer("SSAOKernel", m_pPipeLine->Get_SSAOKernelBuffer());
		m_pShader->Bind_Value("SSAONoiseTexture", { m_pSSAONoiseTexture, "Texture2D", 0 });
		Bind_WorldMatrix();

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

		m_pTargetManager->Bind_Target("Target_SSAO", m_pShader, "SSAOTexture");
		m_pShader->SetConstantBuffer("SSAOBuffer", m_pPipeLine->Get_SSAOBuffer());
		Bind_WorldMatrix();

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

HRESULT CStaticMeshRenderer::Render_StaticMesh_LightAcc()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_LightAcc_Static"))) return E_FAIL;

	m_pTargetManager->Bind_Target("Target_Static_Diffuse", m_pShader, "DiffuseTexture");
	m_pTargetManager->Bind_Target("Target_Static_Normal", m_pShader, "NormalTexture");
	m_pTargetManager->Bind_Target("Target_Static_Depth", m_pShader, "DepthTexture");
	m_pTargetManager->Bind_Target("Target_Static_Metalic", m_pShader, "MetalicTexture");

	Bind_WorldMatrix();
	//m_pPipeLine->Update_ShadowBuffer(m_pContext, -1);
	m_pShader->SetConstantBuffer("ShadowBuffer", m_pPipeLine->Get_ShadowBuffer());
	m_pPipeLine->Bind_ShadowMap(m_pShader);
	m_pPipeLine->Bind_Light(m_pShader, m_pVIBuffer, m_pContext, this);

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	return S_OK;
}

HRESULT CStaticMeshRenderer::Render_StaticMesh_Combined()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Combined_Static"))) return E_FAIL;

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
	//m_pShader->SetConstantBuffer("ShadowBuffer", m_pPipeLine->Get_ShadowBuffer());

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "COMBINED", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pTargetManager->Bind_Target("Target_Static_Diffuse", m_pShader, "DiffuseTexture");
	m_pTargetManager->Bind_Target("Target_SSAO_Blur", m_pShader, "SSAOFinalTexture");
	m_pTargetManager->Bind_Target("Target_Metalic", m_pShader, "MetalicTexture");
	m_pTargetManager->Bind_Target("Target_LightAcc_StaticMesh", m_pShader, "LightTexture");
	m_pTargetManager->Bind_Target("Target_LightInfo_StaticMesh", m_pShader, "LightInfoTexture");
	m_pTargetManager->Bind_Target("Target_BloomBlurY_StaticMesh", m_pShader, "MeshBloomFinalTexture");

	m_pShader->Bind_Value("RampTexture", { m_pRampTexture->Get_SRV(), "Texture2D", 0 });

	Bind_WorldMatrix();

	m_pShader->Apply("COMBINED", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

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

	RenderTargetDesc DiffuseDesc = { "Target_Static_Diffuse" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(DiffuseDesc);

	RenderTargetDesc NormalDesc = { "Target_Static_Normal" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(NormalDesc);

	RenderTargetDesc DepthlDesc = { "Target_Static_Depth" , DXGI_FORMAT_R32G32B32A32_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 1.f, 1.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(DepthlDesc);

	RenderTargetDesc MetalDesc = { "Target_Static_Metalic" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.5f, 1.0f, 1.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(MetalDesc);

	RenderTargetDesc EmissiveDesc = { "Target_Emissive" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(EmissiveDesc);

	RenderTargetDesc EmissiveInfoDesc = { "Target_EmissiveInfo" , DXGI_FORMAT_R16G16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(EmissiveInfoDesc);

	RenderTargetDesc SSAODesc = { "Target_SSAO" , DXGI_FORMAT_R16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(SSAODesc);

	RenderTargetDesc SSAOBlurDesc = { "Target_SSAO_Blur" , DXGI_FORMAT_R16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(SSAOBlurDesc);

	RenderTargetDesc BloomBlurX_StaticMeshDesc = { "Target_BloomBlurX_StaticMesh" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomBlurX_StaticMeshDesc);

	RenderTargetDesc BloomBlurY_StaticMeshDesc = { "Target_BloomBlurY_StaticMesh" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomBlurY_StaticMeshDesc);

	RenderTargetDesc LightAcc_StaticMeshDesc = { "Target_LightAcc_StaticMesh" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(LightAcc_StaticMeshDesc);

	RenderTargetDesc LightInfoDesc = { "Target_LightInfo_StaticMesh" , DXGI_FORMAT_R16G16B16A16_FLOAT  , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(LightInfoDesc);

	RenderTargetDesc Combined_StaticMeshDesc = { "Target_Combined_StaticMesh" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(Combined_StaticMeshDesc);

	return S_OK;
}

HRESULT CStaticMeshRenderer::Ready_MRT()
{
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Static_Diffuse"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Static_Normal"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Static_Depth"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Static_Metalic"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_Emissive"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Static", "Target_EmissiveInfo"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Bloom_Static_H", "Target_BloomBlurX_StaticMesh"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Bloom_Static_V", "Target_BloomBlurY_StaticMesh"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_SSAO", "Target_SSAO"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_SSAO_Blur", "Target_SSAO_Blur"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc_Static", "Target_LightAcc_StaticMesh"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc_Static", "Target_LightInfo_StaticMesh"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Combined_Static", "Target_Combined_StaticMesh"))) return E_FAIL;
	}

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
