#include "Engine_Defines.h"
#include "ForwardRenderer.h"

#include "RenderPass.h"
#include "RenderTarget.h"
#include "Target_Manager.h"
#include "Shader.h"
#include "Texture.h"
#include "PipeLine.h"

CForwardRenderer::CForwardRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CRenderer(pDevice, pContext)
{
}

CForwardRenderer::~CForwardRenderer()
{
}

HRESULT CForwardRenderer::Initialize(CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	__super::Initialize(pTargetManager, pPipeLine);

	LoadShader("Shader_Deferred.hlsl");
	Ready_Target();
	Ready_MRT();
	m_pRampTexture = CTexture::Create(m_pDevice, L"../../DemoResource/RampTexture/RampTexture_2 1.png", "RampTexture", false);

	return S_OK;
}

HRESULT CForwardRenderer::Render_Priority(PriorityPass* pPriorityPass)
{
	m_pPipeLine->Update_FrameBuffer(m_pContext);
	m_pPipeLine->Update_Frustum();

	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Final"))) return E_FAIL;
	pPriorityPass->Execute(m_pContext);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;

	return S_OK;
}

HRESULT CForwardRenderer::Render_Shadow(ShadowPass* pShadowPass)
{
	m_pPipeLine->Update_ShadowBuffer(m_pContext);

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	Change_Viewport(g_iMaxWidth, g_iMaxHeight);
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Shadow"))) return E_FAIL;
	pShadowPass->Execute(m_pContext);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;
	Change_Viewport(ViewportDesc.Width, ViewportDesc.Height);
	return S_OK;
}

HRESULT CForwardRenderer::Render_Forward(OpaquePass* pOpaquePass, InstancePass* pInstancePass)
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Deferred"))) return E_FAIL;
	pOpaquePass->Execute(m_pContext);
	pInstancePass->Execute(m_pContext);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;

	return S_OK;
}

HRESULT CForwardRenderer::Render_LightAcc()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_LightAcc"))) return E_FAIL;

	m_pTargetManager->Bind_Target("Target_Diffuse", m_pShader, "g_DiffuseTexture");
	m_pTargetManager->Bind_Target("Target_Normal", m_pShader, "g_NormalTexture");
	m_pTargetManager->Bind_Target("Target_Depth", m_pShader, "g_DepthTexture");
	m_pTargetManager->Bind_Target("Target_Metalic", m_pShader, "g_MetalicTexture");
	m_pTargetManager->Bind_Target("Target_Ambient", m_pShader, "g_AmbientTexture");

	SHADER_PARAM WorldMat = { &m_WorldMatrix , "float4x4",sizeof(_float4x4) };
	m_pShader->Bind_Value("g_WorldMatrix", WorldMat);
	m_pShader->Bind_Value("g_RampTexture", { m_pRampTexture->Get_SRV(), "Texture2D", 0 });
	m_pPipeLine->Bind_Light(m_pShader, m_pVIBuffer, m_pContext);

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	return S_OK;
}

HRESULT CForwardRenderer::Render_SSAO()
{
	return S_OK;
}

HRESULT CForwardRenderer::Render_Blended(BlendedPass* pBlendPass)
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred");

	ID3D11RenderTargetView* pPrevRTV = { nullptr };
	ID3D11DepthStencilView* pPrevDSV = { nullptr };
	m_pContext->OMGetRenderTargets(1, &pPrevRTV, &pPrevDSV);
	m_pContext->OMSetRenderTargets(1, &pPrevRTV, pDeferredDSV);
	pBlendPass->Execute(m_pContext);
	ID3D11RenderTargetView* pRTVs[8] = { pPrevRTV };
	m_pContext->OMSetRenderTargets(8, pRTVs, pPrevDSV);

	Safe_Release(pPrevRTV);
	Safe_Release(pPrevDSV);

	return S_OK;
}

HRESULT CForwardRenderer::Render_NonLight(NonLightPass* pNonLightPass)
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred");

	ID3D11RenderTargetView* pPrevRTV = { nullptr };
	ID3D11DepthStencilView* pPrevDSV = { nullptr };
	m_pContext->OMGetRenderTargets(1, &pPrevRTV, &pPrevDSV);
	m_pContext->OMSetRenderTargets(1, &pPrevRTV, pDeferredDSV);
	pNonLightPass->Execute(m_pContext);
	ID3D11RenderTargetView* pRTVs[8] = { pPrevRTV };
	m_pContext->OMSetRenderTargets(8, pRTVs, pPrevDSV);

	Safe_Release(pPrevRTV);
	Safe_Release(pPrevDSV);

	return S_OK;
}

HRESULT CForwardRenderer::Ready_Target()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	{
		RenderTargetDesc DiffuseDesc = { "Target_Diffuse" , DXGI_FORMAT_R8G8B8A8_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(DiffuseDesc);

		RenderTargetDesc NormalDesc = { "Target_Normal" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(NormalDesc);

		RenderTargetDesc DepthlDesc = { "Target_Depth" , DXGI_FORMAT_R32G32B32A32_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(DepthlDesc);

		RenderTargetDesc MetalDesc = { "Target_Metalic" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.5f, 1.0f, 1.0f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(MetalDesc);

		RenderTargetDesc AmbiDesc = { "Target_Ambient" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.2f, 0.2f, 0.2f, 1.0f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(AmbiDesc);

		RenderTargetDesc ShadowDesc = { "Target_Shadow" , DXGI_FORMAT_R32G32B32A32_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,g_iMaxWidth, g_iMaxHeight };
		m_pTargetManager->Create_Target(ShadowDesc);

	}

	{
		RenderTargetDesc SSAODesc = { "Target_SSAO" , DXGI_FORMAT_R16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(SSAODesc);

		RenderTargetDesc SSAOBlurDesc = { "Target_SSAO_Blur" , DXGI_FORMAT_R16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(SSAOBlurDesc);
	}

	{
		RenderTargetDesc LightDesc = { "Target_Light" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(LightDesc);
	}

	{
		RenderTargetDesc FianlDesc = { "Target_Final" , DXGI_FORMAT_R8G8B8A8_UNORM ,DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(FianlDesc);
	}

	return S_OK;
}

HRESULT CForwardRenderer::Ready_MRT()
{
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Diffuse"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Normal"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Depth"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Metalic"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Ambient"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Shadow", "Target_Shadow"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc", "Target_Light")))return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_SSAO", "Target_SSAO"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_SSAO_Blur", "Target_SSAO_Blur"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Final", "Target_Final"))) return E_FAIL;
	}

	return S_OK;
}

CForwardRenderer* CForwardRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	CForwardRenderer* Instance = new CForwardRenderer(pDevice, pContext);
	if (FAILED(Instance->Initialize(pTargetManager, pPipeLine)))
	{
		Safe_Release(Instance);
	}
	return Instance;
}

void CForwardRenderer::Free()
{
	__super::Free();
}
