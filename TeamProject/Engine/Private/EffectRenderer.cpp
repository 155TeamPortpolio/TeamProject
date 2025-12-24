#include "Engine_Defines.h"
#include "EffectRenderer.h"

#include "Target_Manager.h"
#include "RenderTarget.h"
#include "RenderPass.h"

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

	LoadShader("Shader_Deferred.hlsl");
	Ready_Target();
	Ready_MRT();

	return S_OK;
}

HRESULT CEffectRenderer::Render_Effect(EffectPass* pEffectPass, ParticlePass* pParticlePass)
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred");

	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Effect", true, pDeferredDSV, false))) return E_FAIL;
	pEffectPass->Execute(m_pContext, this);
	pParticlePass->Execute(m_pContext, this);
	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;
	
	return S_OK;
}

HRESULT CEffectRenderer::Render_WeightOIT()
{


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

void CEffectRenderer::Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command)
{
	m_RenderCommands.push_back(command);
}

HRESULT CEffectRenderer::Ready_Target()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/*------OIT Target-------*/
	RenderTargetDesc AccumulationDesc = { "Target_DiffuseEffectAcc" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(AccumulationDesc);

	RenderTargetDesc BloomAccDesc = { "Target_BloomEffectAcc" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.f, 0.f, 0.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomAccDesc);

	RenderTargetDesc BloomInfoDesc = { "Target_BloomEffectInfo" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomInfoDesc);

	RenderTargetDesc RevealageDesc = { "Target_Revealage" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(RevealageDesc);
	/*-------------------------*/

	RenderTargetDesc DiffuseDesc = { "Target_DiffuseEffect" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(DiffuseDesc);

	RenderTargetDesc BloomDesc = { "Target_BloomEffect" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(BloomDesc);

	RenderTargetDesc DepthlDesc = { "Target_Distortion" , DXGI_FORMAT_R16G16B16A16_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(DepthlDesc);

	return S_OK;
}

HRESULT CEffectRenderer::Ready_MRT()
{
	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_DiffuseEffectAcc")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_BloomEffectAcc")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_Revealage")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_WeightOIT", "Target_BloomEffectInfo")))
			return E_FAIL;
	}

	{
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Effect", "Target_DiffuseEffect"))) 
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Effect", "Target_BloomEffect")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_Effect", "Target_Distortion")))
			return E_FAIL;
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
