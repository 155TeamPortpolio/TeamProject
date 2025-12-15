#include "Engine_Defines.h"
#include "Noise.h"

CNoise::CNoise()
	:IParticleModule()
{
}

void CNoise::SetParams(PARTICLE_MODULE_DESC* pDesc)
{
	NOISE_DESC* pNoiseDesc = static_cast<NOISE_DESC*>(pDesc);


}

void CNoise::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
	m_fElapsedTime += dt;


}

CNoise* CNoise::Create()
{
	CNoise* instance = new CNoise();

	return instance;
}

void CNoise::Free()
{
	__super::Free();
}
