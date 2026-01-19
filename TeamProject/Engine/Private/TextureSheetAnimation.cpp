#include "Engine_Defines.h"
#include "TextureSheetAnimation.h"
#include "Helper_Func.h"

CTextureSheetAnimation::CTextureSheetAnimation()
	:IParticleModule()
{
}

void CTextureSheetAnimation::SetUpParticle(CParticleSystem::PARTICLE& particle)
{
	if (m_IsRandomFrameIndex)
		particle.iFrameIndex = Helper::Get_Random_Int(0, m_iMaxFrameIndex);
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
	m_iMaxFrameIndex = pTextureDesc->iMaxFrameIndex;
}

void CTextureSheetAnimation::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
	if (m_IsParticleAnimated)
	{
		_float t = particle.fLifeTime / particle.fMaxLifeTime;

		particle.iFrameIndex = static_cast<_uint>(t * m_iMaxFrameIndex);
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
