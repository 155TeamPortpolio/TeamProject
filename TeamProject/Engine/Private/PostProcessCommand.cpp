#include "Engine_Defines.h"
#include "PostProcessCommand.h"

#include "PostRenderer.h"
#include "Texture.h"
#include "Helper_Func.h"

CPostProcessCommand* CPostProcessCommand::SetEnable(_bool bEnable)
{
	m_bEnabled = bEnable;
	return this;
}

void CPostProcessCommand::Free()
{
	__super::Free();
}

#pragma region HDRBLOOM
CHDRBloomCommand::CHDRBloomCommand()
{
	m_strName = "HDRBloom";
	m_iPriority = static_cast<_uint>(POST_PROCESS_ORDER::HDRBLOOM);
	m_bEnabled = true;
	m_eEffectType = EFFECT_TYPE::COMBINE;
	m_strOutputTargetName = "Target_HDR_BlurY";
}

CHDRBloomCommand* CHDRBloomCommand::SetIntensity(_float fIntensity)
{
	m_fIntensity = fIntensity;
	return this;
}

void CHDRBloomCommand::Update(_float dt)
{
}

void CHDRBloomCommand::Execute(CPostRenderer* pRenderer)
{
	pRenderer->Render_HDRBloom_Internal();
}

CHDRBloomCommand* CHDRBloomCommand::Create()
{
	return new CHDRBloomCommand();
}

void CHDRBloomCommand::Free()
{
	__super::Free();
}

#pragma endregion

#pragma region GLITCH
CGlitchCommand::CGlitchCommand()
{
	m_strName = "Glitch";
	m_iPriority = static_cast<_uint>(POST_PROCESS_ORDER::GLITCH);
	m_bEnabled = false;
	m_eEffectType = EFFECT_TYPE::REPLACE;
	//m_strOutputTargetName = "Target_Glitch";
}

ID3D11ShaderResourceView* CGlitchCommand::GetNoiseSRV()
{
	return m_pNoiseTexture->Get_SRV();
}

CGlitchCommand* CGlitchCommand::SetNoiseTexture(CTexture* pTexture)
{
	m_pNoiseTexture = pTexture;
	return this;
}

CGlitchCommand* CGlitchCommand::SetDuration(_float fDuration)
{
	m_fDuration = fDuration;
	return this;
}

void CGlitchCommand::Update(_float dt)
{
	m_fAccTime += dt;
}

void CGlitchCommand::Execute(CPostRenderer* pRenderer)
{
	//pRenderer->Render_Glitch_Internal();
}

CGlitchCommand* CGlitchCommand::Create()
{
	return new CGlitchCommand();
}

void CGlitchCommand::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region RADIAL
CRadialBlurCommand::CRadialBlurCommand()
{
	m_strName = "RADIAL";
	m_iPriority = static_cast<_uint>(POST_PROCESS_ORDER::RADIAL_BLUR);
	m_bEnabled = false;
	m_eEffectType = EFFECT_TYPE::REPLACE;
	m_strOutputTargetName = "Target_Radial";
}

CRadialBlurCommand* CRadialBlurCommand::SetCenter(_float2 vCenter)
{
	m_vCenter = vCenter;
	return this;
}

CRadialBlurCommand* CRadialBlurCommand::SetDuration(_float fDuration)
{
	m_fDuration = fDuration;
	m_fAccTime = 0.f;
	return this;
}

CRadialBlurCommand* CRadialBlurCommand::SetIntensity(_float fIntensity)
{
	m_fIntensity = fIntensity;
	return this;
}

CRadialBlurCommand* CRadialBlurCommand::SetEaseType(EaseType easeType)
{
	m_EaseType = easeType;
	return this;
}

void CRadialBlurCommand::Update(_float dt)
{
	m_fAccTime += dt;

	if (m_fAccTime > m_fDuration)
		m_bEnabled = false;

	_float normalizedT = 1.f - (m_fAccTime / m_fDuration);
	_float pingPongT = (normalizedT < 0.5f) ? (normalizedT * 2.f) : (2.f - normalizedT * 2.f);
	m_fEaseT = Math::ApplyEase(m_EaseType, pingPongT);
}

void CRadialBlurCommand::Execute(CPostRenderer* pRenderer)
{
	pRenderer->Render_RadialBlur_Internal();
}

CRadialBlurCommand* CRadialBlurCommand::Create()
{
	return new CRadialBlurCommand();
}

void CRadialBlurCommand::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region FOG
CFogCommand::CFogCommand()
{
	m_strName = "FOG";
	m_iPriority = static_cast<_uint>(POST_PROCESS_ORDER::FOG);
	m_bEnabled = false;
	m_eEffectType = EFFECT_TYPE::REPLACE;
	m_strOutputTargetName = "Target_Fog";
}

void CFogCommand::Update(_float dt)
{
}

void CFogCommand::Execute(CPostRenderer* pRenderer)
{
	pRenderer->Render_Fog_Internal();
}

CFogCommand* CFogCommand::Create()
{
	return new CFogCommand();
}

void CFogCommand::Free()
{
	__super::Free();
}
#pragma endregion

CGuassianBlurCommand::CGuassianBlurCommand()
{
	m_strName = "GuassianBlur";
	m_iPriority = static_cast<_uint>(POST_PROCESS_ORDER::GAUSSIAN_BLUR);
	m_bEnabled = false;
	m_eEffectType = EFFECT_TYPE::REPLACE;
}

CGuassianBlurCommand* CGuassianBlurCommand::SetDuration(_float fDuration)
{
	m_fDuration = fDuration;
	return this;
}

CGuassianBlurCommand* CGuassianBlurCommand::SetIntensity(_float fIntensity)
{
	m_fIntensity = fIntensity;
	return this;
}

void CGuassianBlurCommand::Update(_float dt)
{
	m_fAccTime += dt;
}

void CGuassianBlurCommand::Execute(CPostRenderer* pRenderer)
{
	//
}

CGuassianBlurCommand* CGuassianBlurCommand::Create()
{
	return new CGuassianBlurCommand();
}

void CGuassianBlurCommand::Free()
{
	__super::Free();
}

CAddictiveColorCommand::CAddictiveColorCommand()
{
	m_strName = "AddictiveColor";
	m_iPriority = static_cast<_uint>(POST_PROCESS_ORDER::ADDICTIVE_COLOR);
	m_bEnabled = false;
	m_eEffectType = EFFECT_TYPE::REPLACE;
	m_strOutputTargetName = "Target_AddictiveColor";
}

CAddictiveColorCommand* CAddictiveColorCommand::SetAddictiveColor(_float3* vColor)
{
	m_vAddictiveColor = vColor;
	return this;
}

CPostProcessCommand* CAddictiveColorCommand::SetEnable(_bool bEnable)
{
	m_bEnabled = bEnable;
	if (m_bEnabled == false) m_vAddictiveColor = nullptr;
	return this;
}

void CAddictiveColorCommand::Update(_float dt)
{
}

void CAddictiveColorCommand::Execute(CPostRenderer* pRenderer)
{
	pRenderer->Render_Addictive_Internal();
}

CAddictiveColorCommand* CAddictiveColorCommand::Create()
{
	return new CAddictiveColorCommand();
}

void CAddictiveColorCommand::Free()
{
	__super::Free();
}
