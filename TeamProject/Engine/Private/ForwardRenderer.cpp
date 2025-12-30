#include "Engine_Defines.h"
#include "ForwardRenderer.h"

#include "RenderPass.h"
#include "RenderTarget.h"
#include "Target_Manager.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "PipeLine.h"
#include "Helper_Func.h"

#include "StaticMeshRenderer.h"
#include "SkinnedMeshRenderer.h"

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
	
	m_pStaticRenderer = CStaticMeshRenderer::Create(m_pDevice, m_pContext, m_pTargetManager, m_pPipeLine);
	m_pSkinnedRenderer = CSkinnedMeshRenderer::Create(m_pDevice, m_pContext, m_pTargetManager, m_pPipeLine);

	return S_OK;
}

HRESULT CForwardRenderer::Render_Priority(PriorityPass* pPriorityPass)
{
	m_pPipeLine->Update_FrameBuffer(m_pContext);
	m_pPipeLine->Update_Frustum();

	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Final"))) return E_FAIL;
	pPriorityPass->Execute(m_pContext, this);

	return S_OK;
}

HRESULT CForwardRenderer::Render_Shadow(ShadowPass* pShadowPass)
{
	m_pPipeLine->Update_ShadowBuffer(m_pContext);

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Shadow"))) return E_FAIL;
	Change_Viewport(g_iMaxWidth, g_iMaxHeight);
	pShadowPass->Execute(m_pContext, this);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;
	Change_Viewport(ViewportDesc.Width, ViewportDesc.Height);
	return S_OK;
}

HRESULT CForwardRenderer::Render_StaticMesh(StaticOpaquePass* pOpaquePass, InstancePass* pInstancePass)
{
	if(FAILED(m_pStaticRenderer->Render_StaticMesh(pOpaquePass, pInstancePass))) return E_FAIL;
	return S_OK;
}

HRESULT CForwardRenderer::Render_SkinnedMesh(SkinnedOpaquePass* pOpaquePass)
{
	if (FAILED(m_pSkinnedRenderer->Render_SkinnedMesh(pOpaquePass))) return E_FAIL;
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
	m_pTargetManager->Bind_Target("Target_FaceDir", m_pShader, "g_LookTexture");

	SHADER_PARAM WorldMat = { &m_WorldMatrix , "float4x4",sizeof(_float4x4) };
	m_pShader->Bind_Value("g_WorldMatrix", WorldMat);
	m_pPipeLine->Bind_Light(m_pShader, m_pVIBuffer, m_pContext, this);

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	return S_OK;
}

HRESULT CForwardRenderer::Render_RimLight()
{
	if (FAILED(m_pSkinnedRenderer->Render_RimLight())) return E_FAIL;
	return S_OK;
}

HRESULT CForwardRenderer::Render_SSAO()
{
	if (FAILED(m_pStaticRenderer->Render_SSAO())) return E_FAIL;
	return S_OK;
}

HRESULT CForwardRenderer::Render_OutLine()
{
	m_pSkinnedRenderer->Process_OutLineQueue();
	return S_OK;
}

HRESULT CForwardRenderer::Render_Blended(BlendedPass* pBlendPass)
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred_Static");

	ID3D11RenderTargetView* pPrevRTV = { nullptr };
	ID3D11DepthStencilView* pPrevDSV = { nullptr };
	m_pContext->OMGetRenderTargets(1, &pPrevRTV, &pPrevDSV);
	m_pContext->OMSetRenderTargets(1, &pPrevRTV, pDeferredDSV);
	pBlendPass->Execute(m_pContext, this);
	ID3D11RenderTargetView* pRTVs[8] = { pPrevRTV };
	m_pContext->OMSetRenderTargets(8, pRTVs, pPrevDSV);

	Safe_Release(pPrevRTV);
	Safe_Release(pPrevDSV);

	return S_OK;
}

HRESULT CForwardRenderer::Render_NonLight(NonLightPass* pNonLightPass)
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred_Static");

	ID3D11RenderTargetView* pPrevRTV = { nullptr };
	ID3D11DepthStencilView* pPrevDSV = { nullptr };
	m_pContext->OMGetRenderTargets(1, &pPrevRTV, &pPrevDSV);
	m_pContext->OMSetRenderTargets(1, &pPrevRTV, pDeferredDSV);
	pNonLightPass->Execute(m_pContext, this);
	ID3D11RenderTargetView* pRTVs[8] = { pPrevRTV };
	m_pContext->OMSetRenderTargets(8, pRTVs, pPrevDSV);

	Safe_Release(pPrevRTV);
	Safe_Release(pPrevDSV);

	return S_OK;
}

HRESULT CForwardRenderer::Render_Combined()
{
	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
	m_pShader->SetConstantBuffer("ShadowBuffer", m_pPipeLine->Get_ShadowBuffer());

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "COMBINED", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pTargetManager->Bind_Target("Target_Diffuse", m_pShader, "g_DiffuseTexture");
	m_pTargetManager->Bind_Target("Target_DiffuseEffect", m_pShader, "g_EffectDiffuseTexture");
	m_pTargetManager->Bind_Target("Target_Depth", m_pShader, "g_DepthTexture");
	m_pTargetManager->Bind_Target("Target_SSAO_Blur", m_pShader, "g_SSAOBlurTexture");
	m_pTargetManager->Bind_Target("Target_Metalic", m_pShader, "g_MetalicTexture");
	m_pTargetManager->Bind_Target("Target_Light", m_pShader, "g_LightTexture");
	m_pTargetManager->Bind_Target("Target_LightInfo", m_pShader, "g_LightInfoTexture");
	m_pTargetManager->Bind_Target("Target_Shadow", m_pShader, "g_ShadowTexture");
	m_pTargetManager->Bind_Target("Target_Ambient", m_pShader, "g_AmbientTexture");
	m_pTargetManager->Bind_Target("Target_DiffuseUI", m_pShader, "g_3DUITexture");
	m_pTargetManager->Bind_Target("Target_RimLightFinal", m_pShader, "g_RimLightFinalTexture");
	m_pTargetManager->Bind_Target("Target_Normal", m_pShader, "g_NormalTexture");

	m_pShader->Bind_Value("g_RampTexture", { m_pRampTexture->Get_SRV(), "Texture2D", 0 });

	SHADER_PARAM WorldMat = {};
	WorldMat.iSize = sizeof(_float4x4);
	WorldMat.typeName = "float4x4";
	WorldMat.pData = &m_WorldMatrix;
	m_pShader->Bind_Value("g_WorldMatrix", WorldMat);

	m_pShader->Apply("COMBINED", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	return S_OK;
}

void CForwardRenderer::Add_OutLineCommand(const OUTLINE_COMMAND& command)
{
	m_pSkinnedRenderer->Add_OutLineCommand(command);
}

void CForwardRenderer::Update(_float dt)
{
}

void CForwardRenderer::SetRimLightMode(RIMLIGHT eMode)
{
	m_pSkinnedRenderer->Set_RimLightMode(eMode);
}

HRESULT CForwardRenderer::Ready_Target()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	{
		RenderTargetDesc DiffuseDesc = { "Target_Diffuse" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(DiffuseDesc);

		RenderTargetDesc NormalDesc = { "Target_Normal" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(NormalDesc);

		RenderTargetDesc DepthlDesc = { "Target_Depth" , DXGI_FORMAT_R32G32B32A32_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(DepthlDesc);

		RenderTargetDesc MetalDesc = { "Target_Metalic" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.5f, 1.0f, 1.0f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(MetalDesc);

		RenderTargetDesc ShadowDesc = { "Target_Shadow" , DXGI_FORMAT_R32G32B32A32_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,g_iMaxWidth, g_iMaxHeight };
		m_pTargetManager->Create_Target(ShadowDesc);
	}

	{
		RenderTargetDesc LightDesc = { "Target_Light" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(LightDesc);
		RenderTargetDesc LightInfoDesc = { "Target_LightInfo" , DXGI_FORMAT_R16G16_FLOAT  , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(LightInfoDesc);
	}

	{
		RenderTargetDesc FianlDesc = { "Target_Final" , DXGI_FORMAT_R16G16B16A16_FLOAT ,DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(FianlDesc);
	}

	return S_OK;
}

HRESULT CForwardRenderer::Ready_MRT()
{
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Shadow", "Target_Shadow"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc", "Target_Light")))return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc", "Target_LightInfo")))return E_FAIL;
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

	Safe_Release(m_pRampTexture);

	Safe_Release(m_pSkinnedRenderer);
	Safe_Release(m_pStaticRenderer);
}
