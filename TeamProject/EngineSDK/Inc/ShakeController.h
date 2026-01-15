#pragma once

NS_BEGIN(Engine)

enum class CamShakeType
{
    HitNormal, HitHeavy
};

class ShakeController
{
public:
    void Set(_float ampDeg, _float sustainSec, _float fadeOutSec);
    void Add(_float ampDeg, _float sustainSec, _float fadeOutSec);

    void Set(CamShakeType type, _float strength = 1.f);
    void Add(CamShakeType type, _float strength = 1.f);

    void Clear(_float fadeOutSec);
    void Apply(const Quaternion& camRot, _float dt, Vector3& outWorldPosDelta, Quaternion& outRotDelta);
    void Reset();

private:
    struct PresetKick
    {
        _float rotAmpDeg{};
        _float posAmp{};
        _float sustainSec{};
        _float fadeOutSec{};
        _float kickDurationSec{};
    };

    struct Instance
    {
        _float rotAmpDeg{};
        _float posAmp{};

        _float attackSec{};
        _float sustainSec{};
        _float decaySec{};
        _float elapsed{};

        _float kickDurationSec{};
        _float kickDecaySec{};

        _float yawSign{};
        _float rollSign{};
    };

private:
    static _uint  HashU(_uint x);
    static _float Hash01(_uint x);
    static _float Smooth01(_float t);

    static _float EnvelopeEased(_float elapsed, _float attackSec, _float sustainSec, _float decaySec);
    static _float KickPulse(_float t, _float attackSec, _float durationSec, _float decaySec);
    static _float ClampAttack(_float sustainSec);

    static PresetKick GetPreset(CamShakeType type);

private:
    void AddKick(const PresetKick& p, _float strength);

private:
    vector<Instance> m_instances{};
    _uint            m_seed = 1u;
};

NS_END
