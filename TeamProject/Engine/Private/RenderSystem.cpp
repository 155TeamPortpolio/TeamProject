#include "Engine_Defines.h"
#include "RenderSystem.h"
#include "Model.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "ICameraService.h"
#include "Shader.h"
#include "PipeLine.h"
#include "Material.h"
#include "Target_Manager.h"
#include "IResourceService.h"
#include "Texture.h"
#include "VIBuffer.h"
#include "RenderTarget.h"
#include "EffectRenderer.h"
#include "UIRenderer.h"
#include "PostRenderer.h"
#include "ForwardRenderer.h"
#include "CellBatcher.h"

CRenderSystem::CRenderSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice{ pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}

CRenderSystem::~CRenderSystem()
{
}

HRESULT CRenderSystem::Initialize()
{
	/*Render Target*/
	m_pTargetManager = CTarget_Manager::Create(m_pDevice, m_pContext);

	/*pipeLine*/
	m_pPipeLine = CPipeLine::Create(m_pDevice, this);

	/*RenderPass*/
	m_pPriorityPass = PriorityPass::Create(this);
	m_pStaticPass = StaticOpaquePass::Create(this);
	m_pSkinnedPass = SkinnedOpaquePass::Create(this);
	m_pStaticShadowPass = StaticShadowPass::Create(this);
	m_pSkinnedShadowPass = SkinnedShadowPass::Create(this);
	m_pInstancePass = InstancePass::Create(this);
	m_pBlendedPass = BlendedPass::Create(this);
	m_pParticlePass = ParticlePass::Create(this);
	m_pNonLightPass = NonLightPass::Create(this);
	m_pUIPass = UIPass::Create(this);
	m_pUI3DPass = UI3DPass::Create(this);
	m_pEffectPass = EffectPass::Create(this);


	m_pForward = CForwardRenderer::Create(m_pDevice, m_pContext, m_pTargetManager, m_pPipeLine);
	m_pPost = CPostRenderer::Create(m_pDevice, m_pContext, m_pTargetManager, m_pPipeLine);
	m_pUI = CUIRenderer::Create(m_pDevice, m_pContext, m_pTargetManager, m_pPipeLine);
	m_pEffect = CEffectRenderer::Create(m_pDevice, m_pContext, m_pTargetManager, m_pPipeLine);
	m_pBatcher = CCellBatcher::Create(this);

	return S_OK;
}

HRESULT CRenderSystem::Render()
{
	m_pPipeLine->Begin_ObjectBuffer(m_pContext);
	m_pPipeLine->Begin_SkinningBuffer(m_pContext);

	m_pPriorityPass->Write_Buffer(m_pContext);
	m_pStaticShadowPass->Write_Buffer(m_pContext);
	m_pSkinnedShadowPass->Write_Buffer(m_pContext);
	m_pSkinnedPass->Write_Buffer(m_pContext);
	m_pStaticPass->Write_Buffer(m_pContext);
	m_pInstancePass->Write_Buffer(m_pContext);
	m_pUI3DPass->Write_Buffer(m_pContext);
	m_pEffectPass->Write_Buffer(m_pContext);
	m_pParticlePass->Write_Buffer(m_pContext);
	m_pBlendedPass->Write_Buffer(m_pContext);
	m_pNonLightPass->Write_Buffer(m_pContext);
	m_pUIPass->Write_Buffer(m_pContext);

	m_pPipeLine->End_ObjectBuffer(m_pContext);
	m_pPipeLine->End_SkinningBuffer(m_pContext);


	m_pForward->Render_Priority(m_pPriorityPass);
	m_pForward->Render_StaticShadow(m_pStaticShadowPass, !IsOn);
	m_pForward->Render_SkinnedShadow(m_pSkinnedShadowPass, !IsOn);
	m_pForward->Render_SkinnedMesh(m_pSkinnedPass);
	m_pForward->Render_StaticMesh(m_pStaticPass, m_pInstancePass);

	m_pPipeLine->Update_HiZ(m_pContext);
	m_pUI->Render_3D(m_pUI3DPass);
	m_pEffect->Render_Effect(m_pEffectPass, m_pParticlePass);
	m_pEffect->Render_Effect_Bloom();
	m_pEffect->Render_EffectCombined();

	m_pForward->Render_SSAO();
	m_pForward->Render_LightAcc();
	m_pForward->Render_RimLight();
	m_pForward->Render_Combined();
	m_pForward->Render_Blended(m_pBlendedPass);
	m_pForward->Render_NonLight(m_pNonLightPass);
	m_pForward->Render_OutLine();
	m_pUI->Render_2D(m_pUIPass);

	m_pPost->Render_Fog();
	m_pPost->Render_HDRBloom();
	m_pPost->Render_RadialBlur();
	m_pForward->Render_Bloom();
	m_pPost->Render_Final();


	m_pUI->Render_CustomTarget();

	return S_OK;
}

CRenderSystem* CRenderSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderSystem* Instance = new CRenderSystem(pDevice, pContext);
	if (FAILED(Instance->Initialize()))
	{
		Safe_Release(Instance);
	}
	return Instance;
}

_bool CRenderSystem::Get_FogDesc(FOG_DESC& outResult)
{
	if (!m_pPost)
		return false;

	outResult = m_pPost->Get_FogDesc();
	return true;
}

void CRenderSystem::Set_FogDesc(FOG_DESC desc)
{
	m_pPost->Set_FogDesc(desc);
}

void CRenderSystem::Update(_float dt)
{
	m_pForward->Update(dt);
	m_pPost->Update(dt);
}

CRenderer* CRenderSystem::GetRenderer(RENDERER_TYPE eType)
{
	CRenderer* pRenderer = nullptr;
	switch (eType)
	{
	case RENDERER_TYPE::FORWARD:
		pRenderer = dynamic_cast<CRenderer*>(m_pForward);
		break;
	case RENDERER_TYPE::POST:
		pRenderer = dynamic_cast<CRenderer*>(m_pPost);
		break;
	case RENDERER_TYPE::EFFECT:
		pRenderer = dynamic_cast<CRenderer*>(m_pEffect);
		break;
	case RENDERER_TYPE::UI:
		pRenderer = dynamic_cast<CRenderer*>(m_pUI);
		break;
	}
	return pRenderer;
}

void CRenderSystem::SetRimLightMode(RIMLIGHT eMode)
{
	m_pForward->SetRimLightMode(eMode);
}

void CRenderSystem::Add_NoiseTexture(string strName, CTexture* noiseTexture)
{
	m_pPost->Add_NoiseTexture(strName, noiseTexture);
}

void CRenderSystem::Apply_Noise(vector<string> strNames, _float duration)
{
	m_pPost->Apply_Noise(strNames, duration);
}

void CRenderSystem::Apply_RadialBlur(_float duration, _float2 center)
{
	m_pPost->Apply_RadialBlur(duration, center);
}

void CRenderSystem::Register_AddictiveColor(_float3* pColor)
{
	m_pPost->Register_AddictiveColor(pColor);
}

void CRenderSystem::UnRegister_AddictiveColor()
{
	m_pPost->UnRegister_AddictiveColor();
}

void CRenderSystem::BatchBegin()
{
	_uint FrameIndex = GameInstance()->Get_FrameCount();
	m_pBatcher->BeginBatchFrame(FrameIndex);
}

void CRenderSystem::BatchVisiblePacket(OPAQUE_PACKET& packet)
{
	m_pBatcher->SubmitVisiblePacket(packet);
}

void CRenderSystem::BuildBatchesIfNeeded()
{
	m_pBatcher->BuildBatchesIfNeeded(m_pDevice);
}

_uint CRenderSystem::DrawBatches(RenderPass* pPass, CRenderer* pRenderer)
{
	_uint count = m_pBatcher->DrawBatches(m_pContext, pPass, pRenderer);
	m_pBatcher->EndBatchFrame();
	return count;
}


#ifdef _USING_GUI
void CRenderSystem::Render_GUI()
{
	m_pTargetManager->Render_GUI();
	m_pPipeLine->Render_GUI();
}
#endif // _USING_GUI

#pragma region RenderTarget


void CRenderSystem::Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command, CUSTOMTARGET eCustom)
{
	if (eCustom == CUSTOMTARGET::EFFECT)
	{
		m_pEffect->Add_RenderCommand(command);
		return;
	}
	if (eCustom == CUSTOMTARGET::UI)
	{
		m_pUI->Add_RenderCommand(command);
		return;
	}
}

void CRenderSystem::Add_OutLineCommand(const OUTLINE_COMMAND& command)
{
	m_pForward->Add_OutLineCommand(command);
}

void CRenderSystem::Add_PostProcessCommand(const POST_PROCESS_COMMAND& command)
{
	m_pPost->Add_PostProcessCommand(command);
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

#pragma endregion

void CRenderSystem::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTargetManager);

	Safe_Release(m_pPriorityPass);
	Safe_Release(m_pStaticPass);
	Safe_Release(m_pSkinnedPass);
	Safe_Release(m_pInstancePass);
	Safe_Release(m_pUIPass);
	Safe_Release(m_pUI3DPass);
	Safe_Release(m_pEffectPass);
	Safe_Release(m_pStaticShadowPass);
	Safe_Release(m_pSkinnedShadowPass);
	Safe_Release(m_pBlendedPass);
	Safe_Release(m_pParticlePass);
	Safe_Release(m_pNonLightPass);

	Safe_Release(m_pForward);
	Safe_Release(m_pPost);
	Safe_Release(m_pUI);
	Safe_Release(m_pEffect);
	Safe_Release(m_pBatcher);
}
