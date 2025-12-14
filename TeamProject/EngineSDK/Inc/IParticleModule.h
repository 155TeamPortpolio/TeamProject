#pragma once
#include "Base.h"
#include "ParticleSystem.h"

NS_BEGIN(Engine)
class ENGINE_DLL IParticleModule :
    public CBase
{
public:
    typedef struct tagParticleModuleDesc : public INIT_DESC
    {

    }PARTICLE_MODULE_DESC;
protected:
    virtual ~IParticleModule() DEFAULT;

public:
    virtual void Update(CParticleSystem::PARTICLE& particle, _float dt) PURE;
};
NS_END
