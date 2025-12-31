#include "Engine_Defines.h"
#include "SkinnedMeshRenderer.h"

#include "VIBuffer.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "Target_Manager.h"
#include "Texture.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Helper_Func.h"

CSkinnedMeshRenderer::CSkinnedMeshRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CRenderer(pDevice, pContext)
{
}

CSkinnedMeshRenderer::~CSkinnedMeshRenderer()
{
}

HRESULT CSkinnedMeshRenderer::Initialize(CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	__super::Initialize(pTargetManager, pPipeLine);

	LoadShader("Shader_Deferred_SkinnedMesh.hlsl");
	Ready_Target();
	Ready_MRT();

	return S_OK;
}

HRESULT CSkinnedMeshRenderer::Render_SkinnedMesh(SkinnedOpaquePass* pOpaquePass)
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred_Static");

	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Deferred_Skinned", 0xFF/*, pDeferredDSV, false*/))) return E_FAIL;
	pOpaquePass->Execute(m_pContext, this);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;

	return S_OK;
}

HRESULT CSkinnedMeshRenderer::Render_RimLight()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_RimLightFinal"))) return E_FAIL;

	m_pTargetManager->Bind_Target("Target_RimLight", m_pShader, "RimLightTexture");
	m_pTargetManager->Bind_Target("Target_Skinned_Normal", m_pShader, "NormalTexture");
	m_pTargetManager->Bind_Target("Target_Skinned_Depth", m_pShader, "DepthTexture");

	Bind_WorldMatrix();
	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());

	ID3D11InputLayout* pLayout;
	if (RimLightMode == RIMLIGHT::OUTLINE)
	{
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "OUTLINERIMLIGHT", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("OUTLINERIMLIGHT", m_pContext);
	}
	else
	{
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "RIMLIGHT", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("RIMLIGHT", m_pContext);
	}

	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	return S_OK;
}

HRESULT CSkinnedMeshRenderer::Render_SkinnedMesh_LightAcc()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_LightAcc_Skinned"))) return E_FAIL;

	m_pTargetManager->Bind_Target("Target_Skinned_Diffuse", m_pShader, "DiffuseTexture");
	m_pTargetManager->Bind_Target("Target_Skinned_Normal", m_pShader, "NormalTexture");
	m_pTargetManager->Bind_Target("Target_Skinned_Depth", m_pShader, "DepthTexture");
	m_pTargetManager->Bind_Target("Target_Skinned_Metalic", m_pShader, "MetalicTexture");

	Bind_WorldMatrix();
	m_pPipeLine->Bind_Light(m_pShader, m_pVIBuffer, m_pContext, this);

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	return S_OK;
}

HRESULT CSkinnedMeshRenderer::Render_SkinnedMesh_Combined()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Combined_Skinned"))) return E_FAIL;

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
	//m_pShader->SetConstantBuffer("ShadowBuffer", m_pPipeLine->Get_ShadowBuffer());

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "COMBINED", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	m_pTargetManager->Bind_Target("Target_Skinned_Diffuse", m_pShader, "DiffuseTexture");
	m_pTargetManager->Bind_Target("Target_Ambient", m_pShader, "AmbientTexture");
	m_pTargetManager->Bind_Target("Target_Skinned_Metalic", m_pShader, "MetalicTexture");
	m_pTargetManager->Bind_Target("Target_Skinned_Normal", m_pShader, "NormalTexture");
	m_pTargetManager->Bind_Target("Target_LightAcc_SkinnedMesh", m_pShader, "LightTexture");
	m_pTargetManager->Bind_Target("Target_LightInfo_SkinnedMesh", m_pShader, "LightInfoTexture");
	m_pTargetManager->Bind_Target("Target_RimLightFinal", m_pShader, "RimLightFinalTexture");

	m_pShader->Bind_Value("RampTexture", { m_pRampTexture->Get_SRV(), "Texture2D", 0 });

	Bind_WorldMatrix();

	m_pShader->Apply("COMBINED", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;
	return S_OK;
}

void CSkinnedMeshRenderer::Update(_float dt)
{
}

HRESULT CSkinnedMeshRenderer::Ready_Target()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	{
		RenderTargetDesc DiffuseDesc = { "Target_Skinned_Diffuse" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(DiffuseDesc);

		RenderTargetDesc NormalDesc = { "Target_Skinned_Normal" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(NormalDesc);

		RenderTargetDesc DepthlDesc = { "Target_Skinned_Depth" , DXGI_FORMAT_R32G32B32A32_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(DepthlDesc);

		RenderTargetDesc MetalDesc = { "Target_Skinned_Metalic" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.5f, 1.0f, 1.0f) ,ViewportDesc.Width, ViewportDesc.Height };
		m_pTargetManager->Create_Target(MetalDesc);
	}

	RenderTargetDesc AmbiDesc = { "Target_Ambient" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.2f, 0.2f, 0.2f, 1.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(AmbiDesc);

	RenderTargetDesc FaceDesc = { "Target_FaceDir" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.f, 0.f, 0.f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(FaceDesc);

	RenderTargetDesc RimDesc = { "Target_RimLight" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(RimDesc);

	RenderTargetDesc RimLightDesc = { "Target_RimLightFinal" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(RimLightDesc);

	RenderTargetDesc LightAcc_SkinnedMeshDesc = { "Target_LightAcc_SkinnedMesh" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(LightAcc_SkinnedMeshDesc);

	RenderTargetDesc LightInfoDesc = { "Target_LightInfo_SkinnedMesh" , DXGI_FORMAT_R16G16_FLOAT  , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(LightInfoDesc);

	RenderTargetDesc Combined_SkinnedMeshDesc = { "Target_Combined_SkinnedMesh" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.0f, 0.0f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(Combined_SkinnedMeshDesc);

	return S_OK;
}

HRESULT CSkinnedMeshRenderer::Ready_MRT()
{
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Skinned_Diffuse"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Skinned_Normal"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Skinned_Depth"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Skinned_Metalic"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Ambient"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_RimLight"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_FaceDir"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_RimLightFinal", "Target_RimLightFinal"))) return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc_Skinned", "Target_LightAcc_SkinnedMesh"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc_Skinned", "Target_LightInfo_SkinnedMesh"))) return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Combined_Skinned", "Target_Combined_SkinnedMesh"))) return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkinnedMeshRenderer::Process_OutLineQueue()
{
	if (m_OutLineCommands.empty())
	{
		if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;
		return S_OK;
	}

	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred_Static");

	ID3D11RenderTargetView* pPrevRTV = { nullptr };
	ID3D11DepthStencilView* pPrevDSV = { nullptr };
	m_pContext->OMGetRenderTargets(1, &pPrevRTV, &pPrevDSV);
	m_pContext->OMSetRenderTargets(1, &pPrevRTV, pDeferredDSV);

	for (auto& cmd : m_OutLineCommands)
	{
		cmd.pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
		cmd.pShader->Bind_Value("g_worldMatrix", { cmd.pWorldMatrix, "float4x4", sizeof(_float4x4) });
		cmd.pShader->Bind_Value("g_OutLineBoneMatrices", { cmd.BoneParam.data(), cmd.typeName, cmd.iSize });

		cmd.DrawCall(m_pContext, cmd.MeshIdx);
	}
	ID3D11RenderTargetView* pRTVs[8] = { pPrevRTV };
	m_pContext->OMSetRenderTargets(8, pRTVs, pPrevDSV);

	Safe_Release(pPrevRTV);
	Safe_Release(pPrevDSV);

	m_OutLineCommands.clear();

	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;

	return S_OK;
}

void CSkinnedMeshRenderer::Add_OutLineCommand(const OUTLINE_COMMAND& command)
{
	m_OutLineCommands.push_back(command);
}

CSkinnedMeshRenderer* CSkinnedMeshRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	CSkinnedMeshRenderer* Instance = new CSkinnedMeshRenderer(pDevice, pContext);
	if (FAILED(Instance->Initialize(pTargetManager, pPipeLine)))
	{
		Safe_Release(Instance);
	}
	return Instance;
}

void CSkinnedMeshRenderer::Free()
{
	__super::Free();
}