#include "Engine_Defines.h"
#include "UIRenderer.h"

#include "Target_Manager.h"
#include "RenderPass.h"
#include "GameInstance.h"
#include "RenderTarget.h"

CUIRenderer::CUIRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CRenderer(pDevice, pContext)
{
}

CUIRenderer::~CUIRenderer()
{
}

HRESULT CUIRenderer::Initialize(CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	__super::Initialize(pTargetManager, pPipeLine);
	LoadShader("Shader_Deferred.hlsl");
	Ready_Target();
	Ready_MRT();
	return S_OK;
}

HRESULT CUIRenderer::Render_3D(UI3DPass* pUI3DPass)
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_3DUI"))) return E_FAIL;
	pUI3DPass->Execute(m_pContext, this);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;
	return S_OK;
}

HRESULT CUIRenderer::Render_2D(UIPass* pUIPass)
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_2DUI"))) return E_FAIL;
	pUIPass->Execute(m_pContext, this);
	if (FAILED(m_pTargetManager->End_MRT())) return E_FAIL;
	return S_OK;
}

HRESULT CUIRenderer::Render_Font()
{
	if (FAILED(CGameInstance::GetInstance()->Get_FontSystem()->Render_Font())) return E_FAIL;
	return S_OK;
}

HRESULT CUIRenderer::Render_CustomTarget()
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

void CUIRenderer::Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command)
{
	m_RenderCommands.push_back(command);
}

HRESULT CUIRenderer::Ready_Target()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	RenderTargetDesc UI3D_Desc = { "Target_DiffuseUI", DXGI_FORMAT_R8G8B8A8_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(UI3D_Desc);
	RenderTargetDesc UI2D_Desc = { "Target_UI", DXGI_FORMAT_R8G8B8A8_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT, _float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(UI2D_Desc);
	
	return S_OK;
}

HRESULT CUIRenderer::Ready_MRT()
{
	if (FAILED(m_pTargetManager->Add_MRT("MRT_3DUI", "Target_DiffuseUI"))) return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_2DUI", "Target_UI"))) return E_FAIL;
	return S_OK;
}

CUIRenderer* CUIRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTarget_Manager* pTargetManager, CPipeLine* pPipeLine)
{
	CUIRenderer* Instance = new CUIRenderer(pDevice, pContext);
	if (FAILED(Instance->Initialize(pTargetManager, pPipeLine)))
	{
		Safe_Release(Instance);
	}
	return Instance;
}

void CUIRenderer::Free()
{
	__super::Free();
}
