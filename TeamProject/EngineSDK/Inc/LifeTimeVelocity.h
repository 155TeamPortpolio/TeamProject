#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLifeTimeVelocity :
    public IParticleModule
{
public:
    typedef struct tagLifeTimeVelocityDesc : public IParticleModule::PARTICLE_MODULE_DESC
    {
        _float fDampScale{};
    }LIFE_TIME_VELOCITY_DESC;
private:
    CLifeTimeVelocity();
    virtual ~CLifeTimeVelocity() DEFAULT;

public:
    void SetParams(PARTICLE_MODULE_DESC* pDesc)override;
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CLifeTimeVelocity* Create();
    virtual void Free() override;

private:
    _float m_fDampScale = 10.f;
};
NS_END
