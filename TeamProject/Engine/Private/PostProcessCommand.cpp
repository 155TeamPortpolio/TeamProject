#include "Engine_Defines.h"
#include "PostProcessCommand.h"

void CPostProcessCommand::Free()
{
	__super::Free();
}

#pragma region HDRBLOOM
CHDRBloomCommand::CHDRBloomCommand()
{
	m_strName = "HDRBloom";
	m_iPriority = 100;
	m_bEnabled = true;
}

void CHDRBloomCommand::Update(_float dt)
{
}

void CHDRBloomCommand::Execute(ID3D11DeviceContext* pContext, CRenderTarget* pInput, CRenderTarget* pOutput)
{
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
	m_iPriority = 250;
	m_bEnabled = false;
}

void CGlitchCommand::Update(_float dt)
{
	m_fAccTime += dt;
}

void CGlitchCommand::Execute(ID3D11DeviceContext* pContext, CRenderTarget* pInput, CRenderTarget* pOutput)
{
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
	m_iPriority = 150;
	m_bEnabled = false;
}

void CRadialBlurCommand::Update(_float dt)
{
	m_fAccTime += dt;
}

void CRadialBlurCommand::Execute(ID3D11DeviceContext* pContext, CRenderTarget* pInput, CRenderTarget* pOutput)
{
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

CFogCommand::CFogCommand()
{
	m_strName = "FOG";
	m_iPriority = 50;
	m_bEnabled = false;
}

void CFogCommand::Update(_float dt)
{
}

void CFogCommand::Execute(ID3D11DeviceContext* pContext, CRenderTarget* pInput, CRenderTarget* pOutput)
{
}

CFogCommand* CFogCommand::Create()
{
	return new CFogCommand();
}

void CFogCommand::Free()
{
	__super::Free();
}

CGuassianBlurCommand::CGuassianBlurCommand()
{
	m_strName = "GuassianBlur";
	m_iPriority = 200;
	m_bEnabled = false;
}

void CGuassianBlurCommand::Update(_float dt)
{
}

void CGuassianBlurCommand::Execute(ID3D11DeviceContext* pContext, CRenderTarget* pInput, CRenderTarget* pOutput)
{
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
	m_iPriority = 300;
	m_bEnabled = false;
}

void CAddictiveColorCommand::Update(_float dt)
{
}

void CAddictiveColorCommand::Execute(ID3D11DeviceContext* pContext, CRenderTarget* pInput, CRenderTarget* pOutput)
{
}

CAddictiveColorCommand* CAddictiveColorCommand::Create()
{
	return new CAddictiveColorCommand();
}

void CAddictiveColorCommand::Free()
{
	__super::Free();
}
