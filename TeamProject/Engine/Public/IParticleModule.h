#pragma once
#include "Base.h"
#include "ParticleSystem.h"

NS_BEGIN(Engine)
class IParticleModule :
    public CBase
{
protected:
    virtual ~IParticleModule() DEFAULT;

protected:
    virtual void Update(CParticleSystem::PARTICLE& particle, _float dt) PURE;
};
NS_END
