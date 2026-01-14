#pragma once

NS_BEGIN(Engine)
struct CamPoseFrame;

class ShakeController
{
public:
    void Set(_float ampDeg, _float freq, _float sustainSec, _float fadeOutSec);
    void Add(_float ampDeg, _float freq, _float sustainSec, _float fadeOutSec);
    void Clear(_float fadeOutSec);
    void Apply(const Quaternion& camRot, _float dt, Vector3& outWorldPosDelta, Quaternion& outRotDelta);
    void Reset();

private:
    struct Instance
    {
        _float amplitudeDeg{};
        _float frequency{};
        _float attackSec{};
        _float sustainSec{};
        _float decaySec{};
        _float elapsed{};
        _float posAmp{};
        _uint  seed{};
    };

private:
    static _uint   HashU(_uint x);
    static _float  Hash01(_uint x);
    static _float  Smooth01(_float t);
    static _float  ValueNoise1D(_uint seed, _float x);
    static Vector3 Noise3(_uint seed, _float t);
    static Vector3 FBM3(_uint seed, _float t);
    static _float  Envelope(_float elapsed, _float attackSec, _float sustainSec, _float decaySec);

private:
    vector<Instance> m_instances{};
    _uint            m_seed = 1u;
};

NS_END
