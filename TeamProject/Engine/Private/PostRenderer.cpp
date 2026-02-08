#include "Engine_Defines.h"
#include "PostRenderer.h"

#include "Target_Manager.h"
#include "RenderTarget.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Helper_Func.h"
#include "VIBuffer.h"
#include "Texture.h"

CPostRenderer::CPostRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CRenderer(pDevice, pContext)
{
}

CPostRenderer::~CPostRenderer()
{
}

HRESULT CPostRenderer::Initialize(CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	__super::Initialize(pTargetManager, pPipeLine);

	LoadShader("Shader_Deferred.hlsl");
	Ready_Target();
	Ready_MRT();

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_fScreenWidth = ViewportDesc.Width;
	m_fScreenHeight = ViewportDesc.Height;

	return S_OK;
}

HRESULT CPostRenderer::Render_HDRBloom()
{
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_HDR_Bright"))) return E_FAIL;

		if (fogDesc.IsUse) m_pTargetManager->Bind_Target("Target_Fog", m_pShader, "FinalTexture");
		else m_pTargetManager->Bind_Target("Target_Final", m_pShader, "FinalTexture");

		/* Effect Combined Texture*/
		m_pTargetManager->Bind_Target("Target_Combined_Effect", m_pShader, "EffectCombinedTexture");

		Bind_WorldMatrix();

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "HDR_BRIGHT", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("HDR_BRIGHT", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();
	}

	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_HDR_BlurH"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_HDR_Bright", m_pShader, "HDRBrightTexture");

		Bind_WorldMatrix();

		m_pShader->Bind_Value("fScreenWidth", { &m_fScreenWidth, "float", sizeof(float) });
		m_pShader->Bind_Value("fScreenHeight", { &m_fScreenHeight, "float", sizeof(float) });

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "HDR_BLURH", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("HDR_BLURH", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();

	}

	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_HDR_BlurV"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_HDR_BlurX", m_pShader, "HDRBlurXTexture");

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "HDR_BLURV", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("HDR_BLURV", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();
	}
	return S_OK;
}

HRESULT CPostRenderer::Render_RadialBlur()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_RadialBlur"))) return E_FAIL;

	m_pTargetManager->Bind_Target("Target_Final", m_pShader, "FinalTexture");

	_float normalizedT = 1.f - (m_fRadialDuration / m_fRadialTotalDuration);
	_float pingPongT = (normalizedT < 0.5f) ? (normalizedT * 2.f) : (2.f - normalizedT * 2.f);
	_float EaseT = Math::ApplyEase(EaseType::InOutSine, pingPongT);

	_bool RadialUse = false;
	if (m_fRadialDuration > 0.f) RadialUse = true;

	m_pShader->Bind_Value("g_RadialEaseT", { &EaseT, "float", sizeof(_float) });
	m_pShader->Bind_Value("g_RadialCenter", { &m_fRadialCenter, "float2", sizeof(_float2) });
	m_pShader->Bind_Value("g_RadialUse", { &RadialUse, "bool", sizeof(_bool) });

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "RADIAL", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pShader->Apply("RADIAL", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	m_pTargetManager->End_MRT();
	return S_OK;
}

HRESULT CPostRenderer::Render_Fog()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Fog"))) return E_FAIL;

	m_pTargetManager->Bind_Target("Target_Static_Depth", m_pShader, "StaticDepthTexture");
	m_pTargetManager->Bind_Target("Target_Skinned_Depth", m_pShader, "SkinnedDepthTexture");
	m_pTargetManager->Bind_Target("Target_Final", m_pShader, "FinalTexture");

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

	m_pShader->Bind_Value("g_FogDensity", { &fogDesc.fogDensity, "float", sizeof(_float) });
	m_pShader->Bind_Value("g_FogColor", { &fogDesc.fogColor, "float4", sizeof(_float4) });
	m_pShader->Bind_Value("g_FogUse", { &fogDesc.IsUse, "bool", sizeof(_bool) });

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "FOG", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pShader->Apply("FOG", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	m_pTargetManager->End_MRT();

	return S_OK;
}

HRESULT CPostRenderer::Render_Final()
{
	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "FINAL", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pTargetManager->Bind_Target("Target_Fog", m_pShader, "FinalTexture");
	m_pTargetManager->Bind_Target("Target_DistortionAcc", m_pShader, "DistortionCombinedTexture");
	m_pTargetManager->Bind_Target("Target_Radial", m_pShader, "RadialBloomTexture");
	m_pTargetManager->Bind_Target("Target_MotionNoise", m_pShader, "MotionBlurTexture");
	
	m_pTargetManager->Bind_Target("Target_HDR_BlurY", m_pShader, "HDRBloomFinalTexture");
	m_pTargetManager->Bind_Target("Target_UI", m_pShader, "UI2DTexture");

	m_pTargetManager->Bind_Target("Target_Combined_SkinnedMesh", m_pShader, "SkinnedCombinedTexture");
	m_pTargetManager->Bind_Target("Target_Combined_Effect", m_pShader, "EffectCombinedTexture");

	if (m_pAddictiveColor != nullptr)
	{
		_bool bUseAddictive = _vector3(*m_pAddictiveColor).Length() > 0.01f;

		m_pShader->Bind_Value("g_UseAddictiveColor", { &bUseAddictive, "bool", sizeof(_bool) });
		m_pShader->Bind_Value("g_AddictiveColor", { m_pAddictiveColor, "float3", sizeof(_float3) });
	}
	else
	{
		_bool bUseAddictive = false;
		m_pShader->Bind_Value("g_UseAddictiveColor", { &bUseAddictive, "bool", sizeof(_bool) });
	}
	Bind_WorldMatrix();

	m_pShader->Apply("FINAL", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);
	return S_OK;
}

void CPostRenderer::Update(_float dt)
{
	if (m_fRadialDuration > 0.f) m_fRadialDuration -= dt;
}

HRESULT CPostRenderer::Ready_Target()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	RenderTargetDesc HDRBrightDesc = { "Target_HDR_Bright" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(HDRBrightDesc);

	RenderTargetDesc HDRBlurXDesc = {"Target_HDR_BlurX", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width,ViewportDesc.Height};
	m_pTargetManager->Create_Target(HDRBlurXDesc);

	RenderTargetDesc HDRBlurYDesc = {"Target_HDR_BlurY", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width ,ViewportDesc.Height};
	m_pTargetManager->Create_Target(HDRBlurYDesc);

	RenderTargetDesc RadialBlurDesc = { "Target_Radial", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width ,ViewportDesc.Height };
	m_pTargetManager->Create_Target(RadialBlurDesc);

	RenderTargetDesc FogDesc = {"Target_Fog",DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width,ViewportDesc.Height};
	m_pTargetManager->Create_Target(FogDesc);

	return S_OK;
}

HRESULT CPostRenderer::Ready_MRT()
{
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Fog", "Target_Fog"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_RadialBlur", "Target_Radial"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_HDR_Bright", "Target_HDR_Bright"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_HDR_BlurH", "Target_HDR_BlurX"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_HDR_BlurV", "Target_HDR_BlurY"))) return E_FAIL;
	}

	return S_OK;
}

CPostRenderer* CPostRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	CPostRenderer* Instance = new CPostRenderer(pDevice, pContext);
	if (FAILED(Instance->Initialize(pTargetManager, pPipeLine)))
	{
		Safe_Release(Instance);
	}
	return Instance;
}

void CPostRenderer::Free()
{
	__super::Free();
}
