#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLifeTimeColor :
    public IParticleModule
{
private:
    CLifeTimeColor();
    virtual ~CLifeTimeColor() DEFAULT;

public:
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CLifeTimeColor* Create();
    void Free()override;
};
NS_END
