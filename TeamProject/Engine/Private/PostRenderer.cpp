#include "Engine_Defines.h"
#include "PostRenderer.h"

#include "Target_Manager.h"
#include "RenderTarget.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Helper_Func.h"
#include "VIBuffer.h"
#include "Texture.h"

#include "PostProcessCommand.h"

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

	m_pHDRBloomCommand = CHDRBloomCommand::Create();
	m_pFogCommand = CFogCommand::Create();
	m_pAddictiveColorCommand = CAddictiveColorCommand::Create();
	m_pGlitchCommand = CGlitchCommand::Create();
	m_pRadialBlurCommand = CRadialBlurCommand::Create();
	m_pGuassianBlurCommand = CGuassianBlurCommand::Create();
	m_pSaturationCommand = CSaturationCommand::Create();
	m_pDistortionCommand = CDistortionCommand::Create();

	m_CommandMap[typeid(CHDRBloomCommand)] = m_pHDRBloomCommand;
	m_CommandMap[typeid(CGlitchCommand)] = m_pGlitchCommand;
	m_CommandMap[typeid(CRadialBlurCommand)] = m_pRadialBlurCommand;
	m_CommandMap[typeid(CFogCommand)] = m_pFogCommand;
	m_CommandMap[typeid(CGuassianBlurCommand)] = m_pGuassianBlurCommand;
	m_CommandMap[typeid(CAddictiveColorCommand)] = m_pAddictiveColorCommand;
	m_CommandMap[typeid(CDistortionCommand)] = m_pDistortionCommand;
	m_CommandMap[typeid(CSaturationCommand)] = m_pSaturationCommand;

	return S_OK;
}

HRESULT CPostRenderer::Render_PostProcessCommand()
{
	vector<CPostProcessCommand*> commands = {
		 m_pFogCommand,
		 m_pHDRBloomCommand,
		 m_pGlitchCommand,
		 m_pRadialBlurCommand,
		 m_pGuassianBlurCommand,
		 m_pDistortionCommand,
		 m_pAddictiveColorCommand,
		 m_pSaturationCommand
	};

	commands.erase(
		std::remove(commands.begin(), commands.end(), nullptr),
		commands.end()
	);

	if (commands.empty()) return S_OK;

	std::stable_sort(commands.begin(), commands.end(),
		[](auto* a, auto* b) { return a->GetPriority() < b->GetPriority(); });

	m_strLastTargetName = "Target_Final";
	for (auto& cmd : commands)
	{
		if (!cmd->IsEnabled()) 
			continue;
		cmd->Execute(this);
		if (cmd->GetEffectType() == CPostProcessCommand::EFFECT_TYPE::REPLACE)
			m_strLastTargetName = cmd->GetOutPutTargetName();
	}

	return S_OK;
}

HRESULT CPostRenderer::Render_HDRBloom_Internal()
{
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_HDR_Bright", 0xFF, nullptr, false))) return E_FAIL;

		m_pTargetManager->Bind_Target(m_strLastTargetName, m_pShader, "FinalTexture");

		/* Effect Combined Texture*/
		m_pTargetManager->Bind_Target("Target_Combined_Effect", m_pShader, "EffectCombinedTexture");
		
		m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

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
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_HDR_BlurH", 0xFF, nullptr, false))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_HDR_Bright", m_pShader, "HDRBrightTexture");

		Bind_WorldMatrix();

		m_pShader->Bind_Value("ScreenWidth", { &m_fScreenWidth, "float", sizeof(_float) });
		m_pShader->Bind_Value("ScreenHeight", { &m_fScreenHeight, "float", sizeof(_float) });

		_float fIntensity = m_pHDRBloomCommand->GetIntensity();
		m_pShader->Bind_Value("HDRIntensity", { &fIntensity, "float", sizeof(_float) });

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "HDR_BLURH", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("HDR_BLURH", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();

	}

	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_HDR_BlurV", 0xFF, nullptr, false))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_HDR_BlurX", m_pShader, "HDRBlurXTexture");

		_float fIntensity = m_pHDRBloomCommand->GetIntensity();
		m_pShader->Bind_Value("HDRIntensity", { &fIntensity, "float", sizeof(_float) });

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

HRESULT CPostRenderer::Render_RadialBlur_Internal()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_RadialBlur", 0xFF, nullptr, false))) return E_FAIL;

	m_pTargetManager->Bind_Target(m_strLastTargetName, m_pShader, "FinalTexture");

	_float2 vCenter = m_pRadialBlurCommand->GetCenter();
	_float Intensity = m_pRadialBlurCommand->GetIntensity();

	m_pShader->Bind_Value("RadialCenter", { &vCenter, "float2", sizeof(_float2) });
	m_pShader->Bind_Value("RadialIntensity", { &Intensity, "float", sizeof(_float) });

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "RADIAL", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pShader->Apply("RADIAL", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	m_pTargetManager->End_MRT();
	return S_OK;
}

HRESULT CPostRenderer::Render_Fog_Internal()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Fog", 0xFF, nullptr, false))) return E_FAIL;

	m_pTargetManager->Bind_Target("Target_Static_Depth", m_pShader, "StaticDepthTexture");
	m_pTargetManager->Bind_Target("Target_Skinned_Depth", m_pShader, "SkinnedDepthTexture");
	m_pTargetManager->Bind_Target("Target_DiffuseUI", m_pShader, "UI3DTexture");
	m_pTargetManager->Bind_Target(m_strLastTargetName, m_pShader, "FinalTexture");
	
	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
	
	FOG_DESC fog = m_pFogCommand->GetFogDesc();

	m_pShader->Bind_Value("FogDensity", { &fog.fogDensity, "float", sizeof(_float) });
	m_pShader->Bind_Value("FogColor", { &fog.fogColor, "float4", sizeof(_float4) });
	
	Bind_WorldMatrix();

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "FOG", &pLayout);
	m_pContext->IASetInputLayout(pLayout);
	
	m_pShader->Apply("FOG", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	m_pTargetManager->End_MRT();

	return S_OK;
}

HRESULT CPostRenderer::Render_Addictive_Internal()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Addictive", 0xFF, nullptr, false))) return E_FAIL;

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

	m_pTargetManager->Bind_Target(m_strLastTargetName, m_pShader, "FinalTexture");
	_float3* vAddictiveColor = m_pAddictiveColorCommand->GetAddictiveColor();
	_bool bSkinned = m_pAddictiveColorCommand->GetSkinned();
	m_pShader->Bind_Value("AddictiveColor", { vAddictiveColor, "float3", sizeof(_float3) });
	m_pShader->Bind_Value("bSkinned", { &bSkinned, "bool", sizeof(_bool) });
	m_pTargetManager->Bind_Target("Target_Combined_SkinnedMesh", m_pShader, "SkinnedCombinedTexture");

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "ADDICTIVECOLOR", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pShader->Apply("ADDICTIVECOLOR", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	m_pTargetManager->End_MRT();

	return S_OK;
}

HRESULT CPostRenderer::Render_Glitch_Internal()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Glitch", 0xFF, nullptr, false))) return E_FAIL;

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

	m_pTargetManager->Bind_Target(m_strLastTargetName, m_pShader, "FinalTexture");

	_float fIntensity = m_pGlitchCommand->GetIntensity();
	ID3D11ShaderResourceView* NoiseSRV = m_pGlitchCommand->GetNoiseSRV();
	m_pShader->Bind_Value("GlitchIntensity", { &fIntensity, "float", sizeof(_float) });
	m_pShader->Bind_Value("GlitchNoiseTexture", { NoiseSRV, "Texture2D", 0 });
	m_pShader->Bind_Value("g_Time", { &m_fAccTime, "float", sizeof(_float)});

	Bind_WorldMatrix();

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "GLITCH", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pShader->Apply("GLITCH", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	m_pTargetManager->End_MRT();

	return S_OK;
}

HRESULT CPostRenderer::Render_GuassianBlur_Internal()
{
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_Guassian_BlurH", 0xFF, nullptr, false))) return E_FAIL;

		m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

		m_pTargetManager->Bind_Target(m_strLastTargetName, m_pShader, "FinalTexture");

		_float fIntensity = m_pGuassianBlurCommand->GetIntensity();
		m_pShader->Bind_Value("GuassianIntensity", { &fIntensity, "float", sizeof(_float) });
		m_pShader->Bind_Value("ScreenWidth", { &m_fScreenWidth, "float", sizeof(_float) });
		m_pShader->Bind_Value("ScreenHeight", { &m_fScreenHeight, "float", sizeof(_float) });

		Bind_WorldMatrix();

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "GUASSIAN_BLURH", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("GUASSIAN_BLURH", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();
	}
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_Guassian_BlurV", 0xFF, nullptr, false))) return E_FAIL;

		m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

		m_pTargetManager->Bind_Target("Target_Guassian_BlurX", m_pShader, "GuassianBlurXTexture");

		_float fIntensity = m_pGuassianBlurCommand->GetIntensity();
		m_pShader->Bind_Value("GuassianIntensity", { &fIntensity, "float", sizeof(_float) });

		Bind_WorldMatrix();

		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "GUASSIAN_BLURV", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("GUASSIAN_BLURV", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		m_pTargetManager->End_MRT();
	}
	return S_OK;
}

HRESULT CPostRenderer::Render_Saturation_Internal()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Saturation", 0xFF, nullptr, false))) return E_FAIL;

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

	m_pTargetManager->Bind_Target(m_strLastTargetName, m_pShader, "FinalTexture");

	_float fIntensity = m_pSaturationCommand->GetIntensity();
	m_pShader->Bind_Value("SaturationIntensity", { &fIntensity, "float", sizeof(_float) });

	_uint saturationType = m_pSaturationCommand->GetSaturationType();

	m_pTargetManager->Bind_Target("Target_Combined_StaticMesh", m_pShader, "StaticCombinedTexture");
	m_pTargetManager->Bind_Target("Target_Combined_SkinnedMesh", m_pShader, "SkinnedCombinedTexture");
	m_pTargetManager->Bind_Target("Target_Combined_Effect", m_pShader, "EffectCombinedTexture");

	_bool bSaturateStaticUse = false;
	if (saturationType & static_cast<_uint>(SATURATIONTYPE::STATIC)) 
		bSaturateStaticUse = true;
	m_pShader->Bind_Value("bSaturateStaticUse", { &bSaturateStaticUse, "bool", sizeof(_bool) });

	_bool bSaturateSkinnedUse = false;
	if (saturationType & static_cast<_uint>(SATURATIONTYPE::SKINNED))
		bSaturateSkinnedUse = true;
	m_pShader->Bind_Value("bSaturateSkinnedUse", { &bSaturateSkinnedUse, "bool", sizeof(_bool) });

	_bool bSaturateEffectUse = false;
	if (saturationType & static_cast<_uint>(SATURATIONTYPE::EFFECT))
		bSaturateEffectUse = true;
	m_pShader->Bind_Value("bSaturateEffectUse", { &bSaturateEffectUse, "bool", sizeof(_bool) });

	Bind_WorldMatrix();

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "SATURATION", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pShader->Apply("SATURATION", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	m_pTargetManager->End_MRT();

	return S_OK;
}

HRESULT CPostRenderer::Render_Distortion_Internal()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Distortion", 0xFF, nullptr, false))) return E_FAIL;

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

	m_pTargetManager->Bind_Target(m_strLastTargetName, m_pShader, "FinalTexture");

	Bind_WorldMatrix();

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "DISTORTION", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pShader->Apply("DISTORTION", m_pContext);
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

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

	m_pTargetManager->Bind_Target(m_strLastTargetName, m_pShader, "FinalTexture");
	m_pTargetManager->Bind_Target("Target_DistortionAcc", m_pShader, "DistortionCombinedTexture");

	m_pTargetManager->Bind_Target("Target_HDR_BlurY", m_pShader, "HDRBloomFinalTexture");
	m_pTargetManager->Bind_Target("Target_UI", m_pShader, "UI2DTexture");
	m_pTargetManager->Bind_Target("Target_Combined_Effect", m_pShader, "EffectCombinedTexture");

	Bind_WorldMatrix();

	m_pShader->Apply("FINAL", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);
	return S_OK;
}

void CPostRenderer::Update(_float dt)
{
	m_fAccTime += dt;
	for (auto& cmd : m_CommandMap)
		cmd.second->Update(dt);
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

	RenderTargetDesc FogDesc = {"Target_Fog", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width,ViewportDesc.Height};
	m_pTargetManager->Create_Target(FogDesc);

	RenderTargetDesc AddictiveColorDesc = { "Target_AddictiveColor", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width,ViewportDesc.Height };
	m_pTargetManager->Create_Target(AddictiveColorDesc);

	RenderTargetDesc GlitchDesc = { "Target_Glitch", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width,ViewportDesc.Height };
	m_pTargetManager->Create_Target(GlitchDesc);

	RenderTargetDesc GuassianBlurXDesc = { "Target_Guassian_BlurX", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width,ViewportDesc.Height };
	m_pTargetManager->Create_Target(GuassianBlurXDesc);

	RenderTargetDesc GuassianBlurYDesc = { "Target_Guassian_BlurY", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width ,ViewportDesc.Height };
	m_pTargetManager->Create_Target(GuassianBlurYDesc);

	RenderTargetDesc SaturationDesc = { "Target_Saturation", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width ,ViewportDesc.Height };
	m_pTargetManager->Create_Target(SaturationDesc);

	RenderTargetDesc DistortionDesc = { "Target_Distortion", DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f),ViewportDesc.Width ,ViewportDesc.Height };
	m_pTargetManager->Create_Target(DistortionDesc);

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
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Addictive", "Target_AddictiveColor"))) return E_FAIL;
	}
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Glitch", "Target_Glitch"))) return E_FAIL;
	}
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Saturation", "Target_Saturation"))) return E_FAIL;
	}
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Distortion", "Target_Distortion"))) return E_FAIL;
	}
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_HDR_Bright", "Target_HDR_Bright"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_HDR_BlurH", "Target_HDR_BlurX"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_HDR_BlurV", "Target_HDR_BlurY"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Guassian_BlurH", "Target_Guassian_BlurX"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Guassian_BlurV", "Target_Guassian_BlurY"))) return E_FAIL;
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

	for (auto& cmd : m_CommandMap)
		Safe_Release(cmd.second);
	m_CommandMap.clear();
}
