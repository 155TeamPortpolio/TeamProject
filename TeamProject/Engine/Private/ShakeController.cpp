#include "Engine_Defines.h"
#include "ShakeController.h"
#include "Engine_Math.h"

namespace
{
    _uint HashU(_uint x)
    {
        x ^= x >> 16;
        x *= 0x7feb352d;
        x ^= x >> 15;
        x *= 0x846ca68b;
        x ^= x >> 16;
        return x;
    }

    _float Hash01(_uint x)
    {
        return (HashU(x) & 0x00FFFFFF) / (float)0x01000000;
    }

    _float Smooth01(_float t)
    {
        t = Math::Clamp01(t);
        return t * t * (3.f - 2.f * t);
    }

    _float ValueNoise1D(_uint seed, _float x)
    {
        const _int xi = (_int)floorf(x);
        const _float t = x - (_float)xi;

        const _float a = Hash01(seed ^ (_uint)xi) * 2.f - 1.f;
        const _float b = Hash01(seed ^ (_uint)(xi + 1)) * 2.f - 1.f;

        return a + (b - a) * Smooth01(t);
    }

    Vector3 Noise3(_uint seed, _float t)
    {
        return {
            ValueNoise1D(seed + 11u, t),
            ValueNoise1D(seed + 23u, t),
            ValueNoise1D(seed + 37u, t)
        };
    }

    Vector3 FBM3(_uint seed, _float t)
    {
        Vector3 sum{};
        _float amp = 1.f;
        _float freq = 1.f;

        for (_int i = 0; i < 3; ++i)
        {
            sum += Noise3(seed + (_uint)(i * 101), t * freq) * amp;
            freq *= 2.f;
            amp *= 0.5f;
        }

        return sum;
    }

    _float EnvelopeEased(_float elapsed, _float attackSec, _float sustainSec, _float decaySec)
    {
        if (elapsed < attackSec) return attackSec <= 0.f ? 1.f : Smooth01(elapsed / attackSec);

        elapsed -= attackSec;
        if (elapsed < sustainSec) return 1.f;

        elapsed -= sustainSec;
        if (decaySec <= 0.f) return 0.f;

        return 1.f - Smooth01(elapsed / decaySec);
    }

    _float KickWeight(_float t, _float attackSec, _float durationSec, _float decaySec)
    {
        if (durationSec <= 0.f) return 0.f;
        if (t >= durationSec) return 0.f;

        _float a = 1.f;
        if (t < attackSec) a = attackSec <= 0.f ? 1.f : Smooth01(t / attackSec);

        const _float life = 1.f - Smooth01(t / durationSec);
        const _float d = decaySec <= 0.f ? 1.f : expf(-t / decaySec);

        return a * life * d;
    }

    _float ClampAttack(_float sustainSec)
    {
        const _float a = sustainSec * 0.25f;
        return min(0.04f, max(0.01f, a));
    }

    struct PresetLayer
    {
        _float rotAmpDeg{};
        _float posAmp{};
        _float freq{};
        _float sustainSec{};
        _float fadeOutSec{};

        _float kickAmpDeg{};
        _float kickFreq{};
        _float kickDurationSec{};
    };

    struct Preset
    {
        PresetLayer impact{};
        PresetLayer rumble{};
    };

    Preset GetPreset(CamShakeType type)
    {
        switch (type)
        {
        case CamShakeType::HitNormal:
            return {
                PresetLayer{0.95f, 0.0023f, 26.f, 0.060f, 0.110f, 0.55f, 18.f, 0.090f},
                PresetLayer{0.35f, 0.0008f, 9.f, 0.160f, 0.300f, 0.f, 0.f, 0.f}
            };

        case CamShakeType::HitHeavy:
            return {
                PresetLayer{2.20f, 0.0055f, 18.f, 0.075f, 0.170f, 1.40f, 14.f, 0.130f},
                PresetLayer{0.75f, 0.0018f, 6.8f, 0.220f, 0.420f, 0.f, 0.f, 0.f}
            };
        }

        return {};
    }
}

void ShakeController::Reset()
{
    m_instances.clear();
    m_seed = 1u;
}

void ShakeController::Set(_float ampDeg, _float freq, _float sustainSec, _float fadeOutSec)
{
    m_instances.clear();
    Add(ampDeg, freq, sustainSec, fadeOutSec);
}

void ShakeController::Add(_float ampDeg, _float freq, _float sustainSec, _float fadeOutSec)
{
    const _float posAmp = ampDeg * 0.0025f;
    AddLayer(ampDeg, posAmp, freq, sustainSec, fadeOutSec, 0.f, 0.f, 0.f);
}

void ShakeController::Set(CamShakeType type, _float strength)
{
    m_instances.clear();
    Add(type, strength);
}

void ShakeController::Add(CamShakeType type, _float strength)
{
    const Preset p = GetPreset(type);

    AddLayer(p.impact.rotAmpDeg * strength, p.impact.posAmp * strength, p.impact.freq, p.impact.sustainSec, p.impact.fadeOutSec, p.impact.kickAmpDeg * strength, p.impact.kickFreq, p.impact.kickDurationSec);
    AddLayer(p.rumble.rotAmpDeg * strength, p.rumble.posAmp * strength, p.rumble.freq, p.rumble.sustainSec, p.rumble.fadeOutSec, p.rumble.kickAmpDeg * strength, p.rumble.kickFreq, p.rumble.kickDurationSec);
}

void ShakeController::AddLayer(_float rotAmpDeg, _float posAmp, _float freq, _float sustainSec, _float fadeOutSec, _float kickAmpDeg, _float kickFreq, _float kickDurationSec)
{
    Instance s{};
    s.rotAmpDeg = rotAmpDeg;
    s.posAmp = posAmp;

    s.frequency = freq;
    s.attackSec = ClampAttack(sustainSec);
    s.sustainSec = sustainSec;
    s.decaySec = fadeOutSec;
    s.elapsed = 0.f;

    s.posFreqMul = 1.35f;

    const _uint seed = m_seed++;
    s.seedRot = HashU(seed ^ 0xA341316Cu);
    s.seedPos = HashU(seed ^ 0xC8013EA4u);
    s.rotPhase = Hash01(seed ^ 0x9E3779B9u) * 10.f;
    s.posPhase = Hash01(seed ^ 0x85EBCA6Bu) * 10.f;

    s.kickAmpDeg = kickAmpDeg;
    s.kickFreq = kickFreq;
    s.kickAttackSec = 0.012f;
    s.kickDurationSec = kickDurationSec;
    s.kickDecaySec = max(0.04f, kickDurationSec * 0.55f);

    m_instances.push_back(s);
}

void ShakeController::Clear(_float fadeOutSec)
{
    if (fadeOutSec <= 0.f)
    {
        m_instances.clear();
        return;
    }

    for (auto& s : m_instances)
    {
        const _float sustainEnd = s.attackSec + s.sustainSec;
        if (s.elapsed < sustainEnd) s.sustainSec = max(0.f, s.elapsed - s.attackSec);
        s.decaySec = fadeOutSec;
    }
}

void ShakeController::Apply(const Quaternion& camRot, _float dt, Vector3& outWorldPosDelta, Quaternion& outRotDelta)
{
    outWorldPosDelta = Vector3::Zero;
    outRotDelta = Quaternion::Identity;

    if (m_instances.empty()) return;

    Vector3 rotAcc = Vector3::Zero;
    Vector3 posAcc = Vector3::Zero;

    for (auto& s : m_instances)
    {
        s.elapsed += dt;

        const _float w = EnvelopeEased(s.elapsed, s.attackSec, s.sustainSec, s.decaySec);
        if (w <= 0.f) continue;

        const _bool isImpact = s.kickAmpDeg > 0.f && s.kickDurationSec > 0.f;

        _float evalSec = s.elapsed;
        if (isImpact)
        {
            const _float staccatoHz = 30.f;
            evalSec = floorf(evalSec * staccatoHz) / staccatoHz;
        }

        const _float baseT = evalSec * s.frequency;

        const Vector3 nRot = FBM3(s.seedRot, baseT + s.rotPhase);
        const Vector3 nPos = FBM3(s.seedPos, baseT * s.posFreqMul + s.posPhase);

        const _float rotAmpRad = XMConvertToRadians(s.rotAmpDeg) * w;
        rotAcc += nRot * rotAmpRad;

        const _float posAmp = s.posAmp * w;
        posAcc += nPos * posAmp;

        if (isImpact)
        {
            const _float kw = KickWeight(s.elapsed, s.kickAttackSec, s.kickDurationSec, s.kickDecaySec);
            if (kw > 0.f)
            {
                const _float kickSign = Hash01(s.seedRot ^ 0x68BC21EBu) < 0.5f ? -1.f : 1.f;
                const _float kickRad = XMConvertToRadians(s.kickAmpDeg) * kw * kickSign;

                rotAcc.x += kickRad;
                rotAcc.y += kickRad * 0.18f;
                rotAcc.z += kickRad * 0.10f;
            }
        }
    }

    outRotDelta = Quaternion::CreateFromYawPitchRoll(rotAcc.y * 0.6f, rotAcc.x, rotAcc.z * 0.4f);
    outRotDelta.Normalize();

    const Matrix R = Matrix::CreateFromQuaternion(camRot);
    const Vector3 right = Vector3(R._11, R._12, R._13);
    const Vector3 up = Vector3(R._21, R._22, R._23);
    const Vector3 forward = Vector3(R._31, R._32, R._33);

    outWorldPosDelta = right * posAcc.x + up * posAcc.y + forward * posAcc.z;

    m_instances.erase(remove_if(m_instances.begin(), m_instances.end(), [&](const Instance& s)
        {
            const _float endT = s.attackSec + s.sustainSec + max(0.f, s.decaySec);
            return s.elapsed >= endT;
        }), m_instances.end());
}
