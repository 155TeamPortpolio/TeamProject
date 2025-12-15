#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class CSizeBySpeed :
    public IParticleModule
{
private:
    CSizeBySpeed();
    virtual ~CSizeBySpeed() DEFAULT;

public:
    void SetParams(PARTICLE_MODULE_DESC* pDesc) override;
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CLifeTimeColor* Create();
    void Free()override;

private:

};
NS_END
