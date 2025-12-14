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
    void SetParams(PARTICLE_MODULE_DESC* pDesc)override {};
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CLifeTimeSize* Create();
    void Free() override;

private:
};
NS_END
