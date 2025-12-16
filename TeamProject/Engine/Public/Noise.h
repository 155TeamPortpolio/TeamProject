#pragma once
#include "IParticleModule.h"
#include <PhysX_Inc/foundation/PxPreprocessor.h>

NS_BEGIN(Engine)
class CNoise :
    public IParticleModule
{
public:
    typedef struct tagNoiseDesc : public IParticleModule::PARTICLE_MODULE_DESC
    {
        _float3 vStrength{};
        _float3 vFrequency{};
        _float3 vScrollSpeed{};

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
    _float3 m_vStrength{};
    _float3 m_vFrequency{};
    _float3 m_vScrollSpeed{};

    //_float m_fPositionAmount{};
    //_float m_fRotationAmount{};
    //_float m_fSizeAmount{};

    _float m_fElapsedTime{};

    /*Noise*/
    _float MakeNoise(_float3 p);
    _int Floor(_float x);
    _float Fade(_float t);
    _uint Hash(_uint x);
    _float NormalizeHash(_int x, _int y, _int z);

};
NS_END
