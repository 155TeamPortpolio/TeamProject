#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class CNoise :
    public IParticleModule
{
public:
    typedef struct tagNoiseDesc : public IParticleModule::PARTICLE_MODULE_DESC
    {


    }NOISE_DESC;
private:
    CNoise();
    virtual ~CNoise() DEFAULT;

public:
    void SetParams(PARTICLE_MODULE_DESC* pDesc) override;
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CNoise* Create();
    virtual void Free() override;

private:
    _float3 m_fStrength{};
    _float3 m_fFrequency{};
    _float3 m_fScrollSpeed{};

    _float m_fPositionAmount{};
    _float m_fRotationAmount{};
    _float m_fSizeAmount{};

    _float m_fElapsedTime{};
};
NS_END
