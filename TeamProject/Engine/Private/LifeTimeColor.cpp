#include "Engine_Defines.h"
#include "LifeTimeColor.h"

CLifeTimeColor::CLifeTimeColor()
	:IParticleModule()
{
}

void CLifeTimeColor::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
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
