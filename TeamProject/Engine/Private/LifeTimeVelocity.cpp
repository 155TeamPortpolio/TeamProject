#include "Engine_Defines.h"
#include "LifeTimeVelocity.h"

CLifeTimeVelocity::CLifeTimeVelocity()
	:IParticleModule()
{
}

void CLifeTimeVelocity::SetParams(PARTICLE_MODULE_DESC* pDesc)
{
	LIFE_TIME_VELOCITY_DESC* pVelocityDesc = static_cast<LIFE_TIME_VELOCITY_DESC*>(pDesc);

	m_fDampScale = pVelocityDesc->fDampScale;
}

void CLifeTimeVelocity::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
	_vector3 vVelocity = particle.vVelocity;
	_float fDecay = expf(-m_fDampScale * dt);

	particle.vVelocity = vVelocity * fDecay;
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
