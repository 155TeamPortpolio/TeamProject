#include "Engine_Defines.h"
#include "LifeTimeColor.h"

CLifeTimeColor::CLifeTimeColor()
	:IParticleModule()
{
}

void CLifeTimeColor::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
	_float t = particle.fLifeTime / particle.fMaxLifeTime;

	//_vector4 currColor = _vector4::Lerp(m_StartColor, m_EndColor, t);
	//particle.vColor = currColor;
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
