#include "Engine_Defines.h"
#include "TextureSheetAnimation.h"

CTextureSheetAnimation::CTextureSheetAnimation()
	:IParticleModule()
{
}

void CTextureSheetAnimation::SetParams(PARTICLE_MODULE_DESC* pDesc)
{
	TEXTURE_SHEET_ANIMATION_DESC* pTextureDesc = static_cast<TEXTURE_SHEET_ANIMATION_DESC*>(pDesc);

	m_fFrameSpeed = pTextureDesc->fFrameSpeed;
	m_iCol = pTextureDesc->iCol;
	m_iRow = pTextureDesc->iRow;
}

void CTextureSheetAnimation::Update(CParticleSystem::PARTICLE& particle, _float dt)
{

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
