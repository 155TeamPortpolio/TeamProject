#include "Engine_Defines.h"
#include "LimitVelocity.h"

CLimitVelocity::CLimitVelocity()
	:IParticleModule()
{
}

void CLimitVelocity::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
}

CLimitVelocity* CLimitVelocity::Create()
{
	CLimitVelocity* instance = new CLimitVelocity();

	return instance;
}

void CLimitVelocity::Free()
{
	__super::Free();
}
