#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLifeTimeColor :
    public IParticleModule
{
public:
    typedef struct tagLifeTimeColorDesc : public IParticleModule::PARTICLE_MODULE_DESC
    {
        _float4 vStartColor{};
        _float4 vEndColor{};
    }LIFE_TIME_COLOR_DESC;

private:
    CLifeTimeColor();
    virtual ~CLifeTimeColor() DEFAULT;

public:
    void SetParams(PARTICLE_MODULE_DESC* pDesc)override;
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CLifeTimeColor* Create();
    void Free()override;

private:
    _float4 m_vStartColor{};
    _float4 m_vEndColor{};
};
NS_END
