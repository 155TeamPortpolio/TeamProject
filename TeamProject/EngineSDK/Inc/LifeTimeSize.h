#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLifeTimeSize :
    public IParticleModule
{
private:
    CLifeTimeSize();
    virtual ~CLifeTimeSize() DEFAULT;

public:
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CLifeTimeSize* Create();
    void Free() override;
};
NS_END
