#include "Engine_Defines.h"
#include "LifeTimeSize.h"

CLifeTimeSize::CLifeTimeSize()
	:IParticleModule()
{
}

void CLifeTimeSize::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
	_float t = particle.fLifeTime / particle.fMaxLifeTime;

	_vector2 currSize = _vector2::Lerp(particle.vStartSize, _vector2(0.f, 0.f), t);
	particle.vSize = currSize;
}

CLifeTimeSize* CLifeTimeSize::Create()
{
	CLifeTimeSize* instance = new CLifeTimeSize();

	return instance;
}

void CLifeTimeSize::Free()
{
	__super::Free();
}
