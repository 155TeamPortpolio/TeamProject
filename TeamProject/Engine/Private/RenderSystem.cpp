#include "Engine_Defines.h"
#include "RenderSystem.h"
#include "Model.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "ICameraService.h"
#include "Shader.h"
#include "PipeLine.h"
#include"Material.h"
#include "Target_Manager.h"
#include "IResourceService.h"
#include "VIBuffer.h"
#include "RenderTarget.h"

CRenderSystem::CRenderSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice{ pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(pDevice);
	Safe_AddRef(m_pContext);
}

CRenderSystem::~CRenderSystem()
{
}

HRESULT CRenderSystem::Initialize()
{
	/*pipeLine*/
	m_pPipeLine = CPipeLine::Create(m_pDevice, this);
	/*Render Target*/
	m_pTargetManager = CTarget_Manager::Create(m_pDevice, m_pContext);
	if (FAILED(Ready_GBuffer())) {
		return E_FAIL;
	}

	/*RenderPass*/
	m_pPriorityPass = PriorityPass::Create(this);
	m_pOpaquePass = OpaquePass::Create(this);
	m_pShadowPass = ShadowPass::Create(this);
	m_pInstancePass = InstancePass::Create(this);
	m_pBlendedPass = BlendedPass::Create(this);
	m_pParticlePass = ParticlePass::Create(this);
	m_pUIPass = UIPass::Create(this);

#ifdef _DEBUG
	m_pDebugPass = DebugPass::Create(this);
#endif // _DEBUG



	return S_OK;
}

HRESULT CRenderSystem::Render()
{
	/*FrameBuffer*/
	m_pPipeLine->Update_FrameBuffer(m_pContext);
	m_pPipeLine->Update_Frustum();
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Final"))) return E_FAIL;

	/*Priority*/
	m_pPriorityPass->Execute(m_pContext);
	/*Shadow*/
	Render_Shadow();

	/*ForWard Rendering*/
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Deferred"))) return E_FAIL;
	m_pOpaquePass->Execute(m_pContext);
	m_pInstancePass->Execute(m_pContext);
	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	Render_LightAcc();
	Render_Combined();
	Render_Blended();
	/*Debug Rendering*/

#ifdef _DEBUG
	m_pDebugPass->Execute(m_pContext);
#endif // _DEBUG

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	//Render_Bright();
	/*Custrom Rendering*/
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_UI"))) return E_FAIL;
	/*UI Rendering*/
	m_pUIPass->Execute(m_pContext);
	CGameInstance::GetInstance()->Get_FontSystem()->Render_Font();
	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	Process_RenderCommand();

	if (FAILED(m_pTargetManager->Begin_MRT("MRT_PostProcess"))) return E_FAIL;
	Process_PostProcessQueue();
	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	Render_Final();

	return S_OK;
}

CRenderSystem* CRenderSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderSystem* instance = new CRenderSystem(pDevice, pContext);
	if (FAILED(instance->Initialize())) {
		Safe_Release(instance);
	}
	return instance;
}

HRESULT CRenderSystem::Render_LightAcc()
{
	if (FAILED(m_pTargetManager->Begin_MRT("MRT_LightAcc"))) return E_FAIL;

	SHADER_PARAM NormalParam = {};
	m_pTargetManager->Get_TargetParam("Target_Normal", NormalParam);
	m_pShader->Bind_Value("g_NormalTexture", NormalParam);

	SHADER_PARAM DepthParam = {};
	m_pTargetManager->Get_TargetParam("Target_Depth", DepthParam);
	m_pShader->Bind_Value("g_DepthTexture", DepthParam);

	SHADER_PARAM WorldMat = { &m_WorldMatrix , "float4x4",sizeof(_float4x4) };
	m_pShader->Bind_Value("g_WorldMatrix", WorldMat);

	m_pPipeLine->Bind_Light(m_pShader, m_pVIBuffer, m_pContext);

	if (FAILED(m_pTargetManager->End_MRT()))return E_FAIL;

	return S_OK;
}

HRESULT CRenderSystem::Render_Combined()
{
	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
	m_pShader->SetConstantBuffer("ShadowBuffer", m_pPipeLine->Get_ShadowBuffer());

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "Combined", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	SHADER_PARAM DiffuseParam = {};
	m_pTargetManager->Get_TargetParam("Target_Diffuse", DiffuseParam);
	m_pShader->Bind_Value("g_DiffuseTexture", DiffuseParam);

	SHADER_PARAM DepthParam = {};
	m_pTargetManager->Get_TargetParam("Target_Depth", DepthParam);
	m_pShader->Bind_Value("g_DepthTexture", DepthParam);

	SHADER_PARAM EmmisiveParam = {};
	m_pTargetManager->Get_TargetParam("Target_Emission", EmmisiveParam);
	m_pShader->Bind_Value("g_EmmisiveTexture", EmmisiveParam);

	SHADER_PARAM ShadeParam = {};
	m_pTargetManager->Get_TargetParam("Target_Shade", ShadeParam);
	m_pShader->Bind_Value("g_ShadeTexture", ShadeParam);

	SHADER_PARAM ShadowParam = {};
	m_pTargetManager->Get_TargetParam("Target_Shadow", ShadowParam);
	m_pShader->Bind_Value("g_ShadowTexture", ShadowParam);

	SHADER_PARAM WorldMat = {};
	WorldMat.iSize = sizeof(_float4x4);
	WorldMat.typeName = "float4x4";
	WorldMat.pData = &m_WorldMatrix;
	m_pShader->Bind_Value("g_WorldMatrix", WorldMat);

	m_pShader->Apply("Combined", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);
	return S_OK;
}

HRESULT CRenderSystem::Render_Blended()
{
	ID3D11DepthStencilView* pDeferredDSV =
		m_pTargetManager->Get_MTR_DSV("MRT_Deferred");

	ID3D11RenderTargetView* pPrevRTV = { nullptr };
	ID3D11DepthStencilView* pPrevDSV = { nullptr };
	m_pContext->OMGetRenderTargets(1, &pPrevRTV, &pPrevDSV);
	m_pContext->OMSetRenderTargets(1, &pPrevRTV, pDeferredDSV);
	m_pBlendedPass->Execute(m_pContext);
	m_pParticlePass->Execute(m_pContext);
	ID3D11RenderTargetView* pRTVs[8] = { pPrevRTV };
	m_pContext->OMSetRenderTargets(8, pRTVs, pPrevDSV);

	Safe_Release(pPrevRTV);
	Safe_Release(pPrevDSV);

	return S_OK;
}

HRESULT CRenderSystem::Render_Bright()
{

	m_pShader->SetConstantBuffer("FrameBuffer", m_pPipeLine->Get_FrameBuffer());
	m_pShader->SetConstantBuffer("ShadowBuffer", m_pPipeLine->Get_ShadowBuffer());

	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "Brightness", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	SHADER_PARAM BrightParam = {};
	m_pTargetManager->Get_TargetParam("Target_Emission", BrightParam);
	m_pShader->Bind_Value("g_EmmisiveTexture", BrightParam);

	SHADER_PARAM FinalParam = {};
	m_pTargetManager->Get_TargetParam("Target_Final", FinalParam);
	m_pShader->Bind_Value("g_FinalTexture", FinalParam);

	SHADER_PARAM WorldMat = {};
	WorldMat.iSize = sizeof(_float4x4);
	WorldMat.typeName = "float4x4";
	WorldMat.pData = &m_WorldMatrix;
	m_pShader->Bind_Value("g_WorldMatrix", WorldMat);

	m_pShader->Apply("Brightness", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);
	return S_OK;
}


#ifdef _USING_GUI
void CRenderSystem::Render_GUI()
{
	m_pTargetManager->Render_GUI();
}
#endif // _USING_GUI

#pragma region RenderTarget

HRESULT CRenderSystem::Create_RenderTarget(const RenderTargetDesc& desc)
{
	return 	m_pTargetManager->Create_Target(desc, false);
}

void CRenderSystem::Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command)
{
	m_RenderCommands.push_back(command);
}

void CRenderSystem::Add_PostProcessCommand(const POST_PROCESS_COMMAND& command)
{
	m_PostCommands.push_back(command);
}

void CRenderSystem::DrawTo(const string& targetKey, function<void(ID3D11DeviceContext*)> drawCall)
{
	RENDER_CUSTOM_COMMAND cmd;
	cmd.TargetKey = targetKey;
	cmd.DrawCallback = drawCall;

	m_RenderCommands.push_back(cmd);
}

ID3D11ShaderResourceView* CRenderSystem::Get_CustomTargetSRV(const string strTag)
{
	CRenderTarget* pTarget = m_pTargetManager->Get_CustomTarget(strTag);
	if (!pTarget)
	{
		return nullptr;
	}
	return pTarget->Get_SRV();
}

ID3D11ShaderResourceView* CRenderSystem::Get_EngineTargetSRV(const string strTag)
{
	CRenderTarget* pTarget = m_pTargetManager->Get_EngineTarget(strTag);
	if (!pTarget)
	{
		return nullptr;
	}
	return pTarget->Get_SRV();
}

HRESULT CRenderSystem::Ready_GBuffer()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	RenderTargetDesc DiffuseDesc = { "Target_Diffuse" , DXGI_FORMAT_R8G8B8A8_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(DiffuseDesc);

	RenderTargetDesc NormalDesc = { "Target_Normal" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(NormalDesc);

	RenderTargetDesc DepthlDesc = { "Target_Depth" , DXGI_FORMAT_R32G32B32A32_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(DepthlDesc);

	RenderTargetDesc EmiDesc = { "Target_Emission" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(EmiDesc);

	RenderTargetDesc ShadowDesc = { "Target_Shadow" , DXGI_FORMAT_R32G32B32A32_FLOAT , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(1.f, 1.f, 1.f, 1.f) ,g_iMaxWidth, g_iMaxHeight };
	m_pTargetManager->Create_Target(ShadowDesc);

	RenderTargetDesc ShadeDesc = { "Target_Shade" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(ShadeDesc);

	RenderTargetDesc SpecularDesc = { "Target_Specular" , DXGI_FORMAT_R16G16B16A16_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,ViewportDesc.Width, ViewportDesc.Height };
	m_pTargetManager->Create_Target(SpecularDesc);


	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Emission")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc", "Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc", "Target_Specular")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Shadow", "Target_Shadow")))
		return E_FAIL;

	m_pShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Shader(G_GlobalLevelKey, "Shader_Deferred.hlsl");
	if (nullptr == m_pShader)
		return E_FAIL;
	Safe_AddRef(m_pShader);
	m_pVIBuffer = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(G_GlobalLevelKey, "Engine_Default_Rect", BUFFER_TYPE::BASIC_RECT);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
	Safe_AddRef(m_pVIBuffer);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));


	RenderTargetDesc UI_Desc = { "Target_UI" ,
		DXGI_FORMAT_R8G8B8A8_UNORM , DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,
	ViewportDesc.Width, ViewportDesc.Height };

	m_pTargetManager->Create_Target(UI_Desc);
	if (FAILED(m_pTargetManager->Add_MRT("MRT_UI", "Target_UI")))
		return E_FAIL;

	RenderTargetDesc FianlDesc = { "Target_Final" , DXGI_FORMAT_R8G8B8A8_UNORM ,
			DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,
		ViewportDesc.Width, ViewportDesc.Height };

	m_pTargetManager->Create_Target(FianlDesc);

	if (FAILED(m_pTargetManager->Add_MRT("MRT_Final", "Target_Final")))
		return E_FAIL;

	RenderTargetDesc postProcessDesc = { "Target_PostProcess" , DXGI_FORMAT_R8G8B8A8_UNORM ,
		DXGI_FORMAT_D24_UNORM_S8_UINT,_float4(0.0f, 0.f, 0.f, 0.f) ,
	ViewportDesc.Width, ViewportDesc.Height };

	m_pTargetManager->Create_Target(postProcessDesc);

	if (FAILED(m_pTargetManager->Add_MRT("MRT_PostProcess", "Target_PostProcess")))
		return E_FAIL;

	return S_OK;
}

void CRenderSystem::Process_RenderCommand()
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

		if (pTarget->Get_RTV())
			pTarget->Clear();
		if (pTarget->Get_DSV())
			m_pContext->ClearDepthStencilView(pTarget->Get_DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		cmd.DrawCallback(m_pContext);

		m_pTargetManager->Pop_Target();
	}

	m_RenderCommands.clear();
}

void CRenderSystem::Process_PostProcessQueue()
{
	for (auto& cmd : m_PostCommands)
	{
		m_pTargetManager->Bind_Targets(cmd.TargetNames, cmd.bClearColor, cmd.bClearDepth);

		cmd.DrawCall(m_pContext);

		m_pTargetManager->Restore_Targets();
	}

	m_PostCommands.clear();
}

#pragma endregion

HRESULT CRenderSystem::Get_InputLayout(CModel* pModel, CShader* pShader, _uint DrawIndex, const string& passConstant, ID3D11InputLayout** ppInputLayout)
{
	if (!pModel || !pShader || !ppInputLayout)
		return E_FAIL;

	/*메쉬를 생성할 떄마다 레이아웃을 만드는게아닌, 버텍스구조, 셰이더 구조를 이름으로 아이디화 시켜서 같은거면 그냥 들고 있는걸 쓰는거*/

	/*모델 데이터 이름 + 셰이더 이름*/
	string LayOutID = string(pModel->Get_ElementKey(DrawIndex)) + pShader->Get_Key();

	auto iter = m_InputLayouts.find(LayOutID);

	if (iter != m_InputLayouts.end()) {
		*ppInputLayout = iter->second;
		return S_OK;
	}

	D3DX11_PASS_DESC passDesc = {};

	if (FAILED(pShader->GetPassSignature(passConstant, &passDesc)))
		return E_FAIL;

	if (pModel->Get_ElementCount(DrawIndex) == 0 || pModel->Get_ElementDesc(DrawIndex) == nullptr)
		return E_FAIL;

	HRESULT hr = m_pDevice->CreateInputLayout(
		pModel->Get_ElementDesc(DrawIndex), pModel->Get_ElementCount(DrawIndex),
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize,
		ppInputLayout);

	if (FAILED(hr))
		return E_FAIL;

	m_InputLayouts.emplace(LayOutID, *ppInputLayout);

	return S_OK;
}

HRESULT CRenderSystem::Get_BufferInputLayout(class CVIBuffer* pBuffer, CShader* pShader, const string& passConstant, ID3D11InputLayout** ppInputLayout)
{
	if (!pBuffer || !pShader || !ppInputLayout)
		return E_FAIL;

	string LayOutID = "Buffer_" + pBuffer->Get_Key() + "_" + pShader->Get_Key();

	auto iter = m_InputLayouts.find(LayOutID);

	if (iter != m_InputLayouts.end()) {
		*ppInputLayout = iter->second;
		return S_OK;
	}

	D3DX11_PASS_DESC passDesc = {};

	if (FAILED(pShader->GetPassSignature(passConstant, &passDesc)))
		return E_FAIL;

	if (pBuffer->Get_ElementCount() == 0 || pBuffer->Get_ElementDesc() == nullptr)
		return E_FAIL;

	HRESULT hr = m_pDevice->CreateInputLayout(
		pBuffer->Get_ElementDesc(), pBuffer->Get_ElementCount(),
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize,
		ppInputLayout);

	if (FAILED(hr))
		return E_FAIL;

	m_InputLayouts.emplace(LayOutID, *ppInputLayout);

	return S_OK;
}

void CRenderSystem::Render_Shadow()
{
	m_pPipeLine->Update_ShadowBuffer(m_pContext);

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	if (FAILED(m_pTargetManager->Begin_MRT("MRT_Shadow"))) return;

	Change_Viewport(g_iMaxWidth, g_iMaxHeight);
	m_pShadowPass->Execute(m_pContext);
	m_pTargetManager->End_MRT();
	Change_Viewport(ViewportDesc.Width, ViewportDesc.Height);
}

HRESULT CRenderSystem::Render_Final()
{
	ID3D11InputLayout* pLayout;
	Get_BufferInputLayout(m_pVIBuffer, m_pShader, "Final", &pLayout);
	m_pContext->IASetInputLayout(pLayout);

	SHADER_PARAM finalParam = {};
	m_pTargetManager->Get_TargetParam("Target_Final", finalParam);
	m_pShader->Bind_Value("g_FinalTexture", finalParam);

	SHADER_PARAM uiParam = {};
	m_pTargetManager->Get_TargetParam("Target_UI", uiParam);
	m_pShader->Bind_Value("g_UITexture", uiParam);

	SHADER_PARAM postProcessParam = {};
	m_pTargetManager->Get_TargetParam("Target_PostProcess", postProcessParam);
	m_pShader->Bind_Value("g_PostProcessTexture", postProcessParam);

	SHADER_PARAM WorldMat = {};
	WorldMat.iSize = sizeof(_float4x4);
	WorldMat.typeName = "float4x4";
	WorldMat.pData = &m_WorldMatrix;
	m_pShader->Bind_Value("g_WorldMatrix", WorldMat);

	m_pShader->Apply("Final", m_pContext);
	m_pVIBuffer->Bind_Buffer(m_pContext);
	m_pVIBuffer->Render(m_pContext);

	return S_OK;
}

HRESULT CRenderSystem::Change_Viewport(_uint iWidth, _uint iHeight)
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iWidth;
	ViewPortDesc.Height = (_float)iHeight;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
	return S_OK;
}

void CRenderSystem::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pPipeLine);

	Safe_Release(m_pPriorityPass);
	Safe_Release(m_pOpaquePass);
	Safe_Release(m_pInstancePass);
	Safe_Release(m_pUIPass);
	Safe_Release(m_pDebugPass);
	Safe_Release(m_pShadowPass);
	Safe_Release(m_pBlendedPass);
	Safe_Release(m_pParticlePass);
	Safe_Release(m_pTargetManager);

	for (auto& pair : m_InputLayouts)
		Safe_Release(pair.second);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	m_InputLayouts.clear();
}
