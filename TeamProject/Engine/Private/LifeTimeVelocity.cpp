#include "Engine_Defines.h"
#include "LifeTimeVelocity.h"

CLifeTimeVelocity::CLifeTimeVelocity()
	:IParticleModule()
{
}

void CLifeTimeVelocity::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
}

CLifeTimeVelocity* CLifeTimeVelocity::Create()
{
	CLifeTimeVelocity* instance = new CLifeTimeVelocity();

	return instance;
}

void CLifeTimeVelocity::Free()
{
	__super::Free();
}
