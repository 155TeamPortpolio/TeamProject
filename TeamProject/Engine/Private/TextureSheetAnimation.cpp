#include "Engine_Defines.h"
#include "TextureSheetAnimation.h"
#include "Helper_Func.h"

CTextureSheetAnimation::CTextureSheetAnimation()
	:IParticleModule()
{
}

void CTextureSheetAnimation::SetUpParticle(CParticleSystem::PARTICLE& particle)
{
	_uint iMaxFrames = m_iCol * m_iRow;

	if (m_IsRandomFrameIndex)
		particle.iFrameIndex = Helper::Get_Random_Int(0, iMaxFrames - 1);
	else
		particle.iFrameIndex = 0;
}

void CTextureSheetAnimation::SetParams(PARTICLE_MODULE_DESC* pDesc)
{
	TEXTURE_SHEET_ANIMATION_DESC* pTextureDesc = static_cast<TEXTURE_SHEET_ANIMATION_DESC*>(pDesc);

	m_IsParticleAnimated = pTextureDesc->isParticleAnimated;
	m_IsRandomFrameIndex = pTextureDesc->isRandomFrameIndex;
	m_iCol = pTextureDesc->iCol;
	m_iRow = pTextureDesc->iRow;
}

void CTextureSheetAnimation::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
	if (m_IsParticleAnimated)
	{

	}
}

CTextureSheetAnimation* CTextureSheetAnimation::Create()
{
	CTextureSheetAnimation* instance = new CTextureSheetAnimation();

	return instance;	
}

void CTextureSheetAnimation::Free()
{
	__super::Free();
}
