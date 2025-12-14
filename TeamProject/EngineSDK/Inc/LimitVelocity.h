#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLimitVelocity :
    public IParticleModule
{
private:
    CLimitVelocity();
    virtual ~CLimitVelocity() DEFAULT;

public:
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CLimitVelocity* Create();
    virtual void Free() override;
};
NS_END
