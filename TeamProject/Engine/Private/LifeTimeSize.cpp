#include "Engine_Defines.h"
#include "LifeTimeSize.h"

CLifeTimeSize::CLifeTimeSize()
{
}

void CLifeTimeSize::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
}

CLifeTimeSize* CLifeTimeSize::Create()
{
	CLifeTimeSize* instance = new CLifeTimeSize();

	return instance;
}

void CLifeTimeSize::Free()
{
}
