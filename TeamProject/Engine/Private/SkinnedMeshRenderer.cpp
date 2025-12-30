#include "Engine_Defines.h"
#include "SkinnedMeshRenderer.h"

#include "VIBuffer.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "Target_Manager.h"
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

	LoadShader("Shader_Deferred.hlsl");
	Ready_Target();
	Ready_MRT();

	return S_OK;
}

HRESULT CSkinnedMeshRenderer::Render_SkinnedMesh(SkinnedOpaquePass* pOpaquePass)
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred_Static");

	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Deferred_Skinned", 0x30, pDeferredDSV, false))) return E_FAIL;
	pOpaquePass->Execute(m_pContext, this);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;

	return S_OK;
}

HRESULT CSkinnedMeshRenderer::Render_RimLight()
{
	if (RimLightMode == RIMLIGHT::OUTLINE)
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_RimLightFinal"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_RimLight", m_pShader, "g_RimLightTexture");
		m_pTargetManager->Bind_Target("Target_Normal", m_pShader, "g_NormalTexture");
		m_pTargetManager->Bind_Target("Target_Depth", m_pShader, "g_DepthTexture");

		SHADER_PARAM WorldMat = { &m_WorldMatrix , "float4x4",sizeof(_float4x4) };
		m_pShader->Bind_Value("g_WorldMatrix", WorldMat);
		m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "OUTLINERIMLIGHT", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("OUTLINERIMLIGHT", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;
	}
	else if (RimLightMode == RIMLIGHT::BACKLIGHT)
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_RimLightFinal"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_RimLight", m_pShader, "g_RimLightTexture");
		m_pTargetManager->Bind_Target("Target_Normal", m_pShader, "g_NormalTexture");
		m_pTargetManager->Bind_Target("Target_Depth", m_pShader, "g_DepthTexture");

		SHADER_PARAM WorldMat = { &m_WorldMatrix , "float4x4",sizeof(_float4x4) };
		m_pShader->Bind_Value("g_WorldMatrix", WorldMat);
		m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "BACKRIMLIGHT", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("BACKRIMLIGHT", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;
	}
	else if (RimLightMode == RIMLIGHT::RIMLIGHT)
	{
		if (FAILED(m_pTargetManager->Begin_MRT("MRT_RimLightFinal"))) return E_FAIL;

		m_pTargetManager->Bind_Target("Target_RimLight", m_pShader, "g_RimLightTexture");
		m_pTargetManager->Bind_Target("Target_Normal", m_pShader, "g_NormalTexture");
		m_pTargetManager->Bind_Target("Target_Depth", m_pShader, "g_DepthTexture");

		SHADER_PARAM WorldMat = { &m_WorldMatrix , "float4x4",sizeof(_float4x4) };
		m_pShader->Bind_Value("g_WorldMatrix", WorldMat);
		m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
		ID3D11InputLayout* pLayout;
		Get_BufferInputLayout(m_pVIBuffer, m_pShader, "RIMLIGHT", &pLayout);
		m_pContext->IASetInputLayout(pLayout);

		m_pShader->Apply("RIMLIGHT", m_pContext);
		m_pVIBuffer->Bind_Buffer(m_pContext);
		m_pVIBuffer->Render(m_pContext);

		if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;
	}

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

	RenderTargetDesc AmbiDesc = { "Target_Ambient" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.2f, 0.2f, 0.2f, 1.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(AmbiDesc);

	RenderTargetDesc FaceDesc = { "Target_FaceDir" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.f, 0.f, 0.f, 0.0f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(FaceDesc);

	RenderTargetDesc RimDesc = { "Target_RimLight" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(RimDesc);

	RenderTargetDesc RimLightDesc = { "Target_RimLightFinal" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(RimLightDesc);

	return S_OK;
}

HRESULT CSkinnedMeshRenderer::Ready_MRT()
{
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Diffuse"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Normal"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Depth"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Metalic"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_Ambient"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_RimLight"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred_Skinned", "Target_FaceDir"))) return E_FAIL;

	if (FAILED(m_pTargetManager->Add_MRT("MRT_RimLightFinal", "Target_RimLightFinal"))) return E_FAIL;

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