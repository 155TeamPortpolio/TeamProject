#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLifeTimeSize :
    public IParticleModule
{
public:
    typedef struct tagLifeTimeSizeDesc : public IParticleModule::PARTICLE_MODULE_DESC
    {
        _float2 vStartScale{};
        _float2 vEndScale{};
    }LIFE_TIME_SIZE_DESC;

private:
    CLifeTimeSize();
    virtual ~CLifeTimeSize() DEFAULT;

public:
    void SetParams(PARTICLE_MODULE_DESC* pDesc)override;
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CLifeTimeSize* Create();
    void Free() override;

private:
    _float2 m_vStartScale{ 1.f,1.f };
    _float2 m_vEndScale{ 1.f,1.f };
};
NS_END
