#include "Engine_Defines.h"
#include "EffectRenderer.h"
#include "Target_Manager.h"
#include "RenderTarget.h"
#include "RenderPass.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "PipeLine.h"

CEffectRenderer::CEffectRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CRenderer(pDevice, pContext)
{
}

CEffectRenderer::~CEffectRenderer()
{
}

HRESULT CEffectRenderer::Initialize(CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	__super::Initialize(pTargetManager, pPipeLine);

	LoadShader("Shader_Deferred_Effect.hlsl");
	Ready_Target();
	Ready_MRT();

	return S_OK;
}

HRESULT CEffectRenderer::Render_Effect(EffectPass* pEffectPass, ParticlePass* pParticlePass)
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred_Skinned");

	/* Color Acc Pass */
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_WeightOIT", 0xff, pDeferredDSV, false))) return E_FAIL;
		pEffectPass->Execute(m_pContext, this);
		pParticlePass->Execute(m_pContext, this);
		if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;
	}

	/* Composite Pass */
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_Effect", 0xff, pDeferredDSV, false))) return E_FAIL;

		SHADER_PARAM WorldMat = {};
		WorldMat.iSize = sizeof(_float4x4);
		WorldMat.typeName = "float4x4";
		WorldMat.pData = &m_WorldMatrix;

		m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
		m_pShader->Bind_Value("g_WorldMatrix", WorldMat);
		
		m_pShader->Bind_Value("BloomScreenWidth", { &m_fBloomScreenWidth, "float", sizeof(_float) });
		m_pShader->Bind_Value("BloomScreenHeight", { &m_fBloomScreenHeight, "float", sizeof(_float) });
		m_pTargetManager->Bind_Target("Target_DiffuseEffectAcc", m_pShader, "EffectAccTexture");
		m_pTargetManager->Bind_Target("Target_BloomEffectAcc", m_pShader, "EffectBloomAccTextutre");
		m_pTargetManager->Bind_Target("Target_Revealage", m_pShader, "RevealageTexture");
		m_pTargetManager->Bind_Target("Target_RimLightAcc", m_pShader, "RimLightAccTexture");
		
		ID3D11InputLayout* pLayout = nullptr;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "Composite", &pLayout);
		m_pContext->IASetInputLayout(pLayout);
		
		m_pShader->Apply("COMPOSITE", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);
		
		if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffectRenderer::Render_EffectCombined()
{
	/* Combined Pass */
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Combined_Effect"))) return E_FAIL;

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "COMBINED", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pTargetManager->Bind_Target("Target_DiffuseEffect", m_pShader, "DiffuseTexture");
	m_pTargetManager->Bind_Target("Target_BloomBlurY_Effect", m_pShader, "EffectBloomFinalTexture");

	Bind_WorldMatrix();

	m_pShader->Apply("COMBINED", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	return S_OK;
}

HRESULT CEffectRenderer::Render_CustomTarget()
{
	for (auto& cmd : m_RenderCommands)
	{
		CRenderTarget* pTarget = m_pTargetManager->Get_CustomTarget(cmd.TargetKey);

		if (!pTarget)
		{
			MSG_BOX("Invalid RenderCommand Target Key");
			continue;
		}

		m_pTargetManager->Push_Target(cmd.TargetKey);

		if (pTarget->Get_RTV()) pTarget->Clear();
		if (pTarget->Get_DSV()) m_pContext->ClearDepthStencilView(pTarget->Get_DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		cmd.DrawCallback(m_pContext);

		m_pTargetManager->Pop_Target();
	}

	m_RenderCommands.clear();

	return S_OK;
}

HRESULT CEffectRenderer::Render_Effect_Bloom()
{
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_Bloom_Effect_H"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_BloomEffect", m_pShader, "EffectBrightTexture");
		m_pTargetManager->Bind_Target("Target_BloomEffectInfo", m_pShader, "EffectBloomInfo");

		Bind_WorldMatrix();

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "BLOOM_BLURX", &pLayout);
		m_pContext->IASetInputLayout(pLayout);


		m_pShader->Bind_Value("BloomScreenWidth", { &m_fBloomScreenWidth, "float", sizeof(float) });
		m_pShader->Bind_Value("BloomScreenHeight", { &m_fBloomScreenHeight, "float", sizeof(float) });

		m_pShader->Apply("BLOOM_BLURX", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();
	}

	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_Bloom_Effect_V"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_BloomBlurX_Effect", m_pShader, "EffectBlurXTexture");
		m_pTargetManager->Bind_Target("Target_BloomEffectInfo", m_pShader, "EffectBloomInfo");
		
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

void CEffectRenderer::Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command)
{
	m_RenderCommands.push_back(command);
}

HRESULT CEffectRenderer::Ready_Target()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_fBloomScreenWidth = ViewportDesc.Width;
	m_fBloomScreenHeight = ViewportDesc.Height;

	RenderTargetDesc AccumulationDesc = { "Target_DiffuseEffectAcc" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(AccumulationDesc);

	RenderTargetDesc BloomAccDesc = { "Target_BloomEffectAcc" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomAccDesc);

	RenderTargetDesc BloomInfoDesc = { "Target_BloomEffectInfo" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomInfoDesc);

	RenderTargetDesc RevealageDesc = { "Target_Revealage" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(RevealageDesc);

	RenderTargetDesc DistortionAccDesc = { "Target_DistortionAcc",DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.f,0.f,0.f,0.f), ViewportDesc.Width,ViewportDesc.Height };
	m_pTargetManager->Create_Target(DistortionAccDesc);

	RenderTargetDesc RimLightAccDesc = { "Target_RimLightAcc",DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.f,0.f,0.f,0.f),ViewportDesc.Width,ViewportDesc.Height };
	m_pTargetManager->Create_Target(RimLightAccDesc);

	RenderTargetDesc DiffuseDesc = { "Target_DiffuseEffect" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(DiffuseDesc);

	RenderTargetDesc BloomDesc = { "Target_BloomEffect" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomDesc);

	RenderTargetDesc BloomBlurX_EffectDesc = { "Target_BloomBlurX_Effect" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomBlurX_EffectDesc);

	RenderTargetDesc BloomBlurY_EffectDesc = { "Target_BloomBlurY_Effect" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomBlurY_EffectDesc);

	RenderTargetDesc Combined_EffectDesc = { "Target_Combined_Effect", DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(Combined_EffectDesc);

	return S_OK;
}

HRESULT CEffectRenderer::Ready_MRT()
{
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_DiffuseEffectAcc")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_BloomEffectAcc")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_BloomEffectInfo")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_Revealage")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_DistortionAcc")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_RimLightAcc")))
			return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Effect", "Target_DiffuseEffect")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Effect", "Target_BloomEffect")))
			return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Bloom_Effect_H", "Target_BloomBlurX_Effect"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Bloom_Effect_V", "Target_BloomBlurY_Effect"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Combined_Effect", "Target_Combined_Effect"))) return E_FAIL;
	}
	return S_OK;
}

CEffectRenderer* CEffectRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	CEffectRenderer* Instance = new CEffectRenderer(pDevice, pContext);
	if (FAILED(Instance->Initialize(pTargetManager, pPipeLine)))
	{
		Safe_Release(Instance);
	}
	return Instance;
}

void CEffectRenderer::Free()
{
	__super::Free();
}
