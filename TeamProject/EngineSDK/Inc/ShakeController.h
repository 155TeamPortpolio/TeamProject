#pragma once

NS_BEGIN(Engine)

enum class CamShakeType 
{
    HitNormal, HitHeavy
};

class ShakeController
{
public:
    void Set(_float ampDeg, _float freq, _float sustainSec, _float fadeOutSec);
    void Add(_float ampDeg, _float freq, _float sustainSec, _float fadeOutSec);

    void Set(CamShakeType type, _float strength = 1.f);
    void Add(CamShakeType type, _float strength = 1.f);

    void Clear(_float fadeOutSec);
    void Apply(const Quaternion& camRot, _float dt, Vector3& outWorldPosDelta, Quaternion& outRotDelta);
    void Reset();

private:
    struct Instance
    {
        _float rotAmpDeg{};
        _float posAmp{};

        _float frequency{};
        _float attackSec{};
        _float sustainSec{};
        _float decaySec{};
        _float elapsed{};

        _float posFreqMul{};
        _float rotPhase{};
        _float posPhase{};

        _float kickAmpDeg{};
        _float kickFreq{};
        _float kickAttackSec{};
        _float kickDurationSec{};
        _float kickDecaySec{};

        _uint  seedRot{};
        _uint  seedPos{};
    };

private:
    void AddLayer(_float rotAmpDeg, _float posAmp, _float freq, _float sustainSec, _float fadeOutSec, _float kickAmpDeg, _float kickFreq, _float kickDurationSec);

private:
    vector<Instance> m_instances{};
    _uint            m_seed = 1u;
};

NS_END
