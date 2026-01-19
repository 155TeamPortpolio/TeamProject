#include "Engine_Defines.h"
#include "LifeTimeColor.h"

CLifeTimeColor::CLifeTimeColor()
	:IParticleModule()
{
}

void CLifeTimeColor::SetParams(PARTICLE_MODULE_DESC* pDesc)
{
	LIFE_TIME_COLOR_DESC* pColorDesc = static_cast<LIFE_TIME_COLOR_DESC*>(pDesc);

	m_vStartColor = pColorDesc->vStartColor;
	m_vEndColor = pColorDesc->vEndColor;
}

void CLifeTimeColor::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
	_float t = particle.fLifeTime / particle.fMaxLifeTime;

	_vector4 currColor = _vector4::Lerp(m_vStartColor, m_vEndColor, t);
	particle.vColor = currColor;
}

CLifeTimeColor* CLifeTimeColor::Create()
{
	CLifeTimeColor* instance = new CLifeTimeColor();

	return instance;
}

void CLifeTimeColor::Free()
{
	__super::Free();
}
