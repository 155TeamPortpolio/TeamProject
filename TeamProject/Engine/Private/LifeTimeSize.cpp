#include "Engine_Defines.h"
#include "LifeTimeSize.h"

CLifeTimeSize::CLifeTimeSize()
	:IParticleModule()
{
}

void CLifeTimeSize::SetParams(PARTICLE_MODULE_DESC* pDesc)
{
	LIFE_TIME_SIZE_DESC* pSizeDesc = static_cast<LIFE_TIME_SIZE_DESC*>(pDesc);

	m_vStartScale = pSizeDesc->vStartScale;
	m_vEndScale = pSizeDesc->vEndScale;
}

void CLifeTimeSize::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
	_float t = particle.fLifeTime / particle.fMaxLifeTime;

	_vector2 currScale = _vector2::Lerp(m_vStartScale, m_vEndScale, t);
	particle.vSize = currScale * particle.vStartSize;
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
