#include "Engine_Defines.h"
#include "ShakeController.h"
#include "Engine_Math.h"

namespace
{
    constexpr _float kPi = 3.14159265358979323846f;
    constexpr _float kDegToRad = kPi / 180.f;

    static const CamShakePreset kPresets[(int)CamShakeType::End] =
    {
        CamShakePreset{0.10f, 0.00030f, 0.050f, 0.06f, 0.00018f, 16.f, 0.080f, 0.140f},
        CamShakePreset{0.16f, 0.00042f, 0.055f, 0.08f, 0.00022f, 16.f, 0.090f, 0.160f},
        CamShakePreset{0.22f, 0.00060f, 0.060f, 0.10f, 0.00030f, 15.f, 0.100f, 0.180f},
        CamShakePreset{0.40f, 0.00095f, 0.070f, 0.18f, 0.00048f, 14.f, 0.120f, 0.220f},
        CamShakePreset{0.55f, 0.00115f, 0.075f, 0.22f, 0.00060f, 13.f, 0.135f, 0.260f},

        CamShakePreset{0.32f, 0.00105f, 0.080f, 0.16f, 0.00055f, 12.f, 0.140f, 0.260f},
        CamShakePreset{0.75f, 0.00185f, 0.085f, 0.30f, 0.00095f, 11.f, 0.160f, 0.320f},

        CamShakePreset{0.20f, 0.00095f, 0.070f, 0.08f, 0.00035f, 12.f, 0.110f, 0.220f},
        CamShakePreset{0.48f, 0.00155f, 0.080f, 0.14f, 0.00055f, 11.f, 0.140f, 0.300f},

        CamShakePreset{0.12f, 0.00040f, 0.060f, 0.10f, 0.00025f, 18.f, 0.080f, 0.140f},
    };

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

    _float Clamp01(_float t)
    {
        if (t < 0.f) return 0.f;
        if (t > 1.f) return 1.f;
        return t;
    }

    _float Smooth01(_float t)
    {
        t = Clamp01(t);
        return t * t * (3.f - 2.f * t);
    }

    _float DegToRad(_float deg)
    {
        return deg * kDegToRad;
    }

    _float Envelope(_float elapsed, _float attackSec, _float sustainSec, _float decaySec)
    {
        if (elapsed < attackSec) return attackSec <= 0.f ? 1.f : Smooth01(elapsed / attackSec);

        elapsed -= attackSec;
        if (elapsed < sustainSec) return 1.f;

        elapsed -= sustainSec;
        if (decaySec <= 0.f) return 0.f;

        return 1.f - Smooth01(elapsed / decaySec);
    }

    _float KickCurve(_float t, _float kickDur)
    {
        if (kickDur <= 0.f) return 0.f;
        if (t >= kickDur) return 0.f;

        const _float u = Clamp01(t / kickDur);
        const _float a = Smooth01(min(1.f, u / 0.20f));
        const _float s = sinf(u * kPi);
        return a * s;
    }

    _float Noise3(_float t, _float f, _float p0, _float p1, _float p2)
    {
        const _float x0 = sinf((t + p0) * f * (2.f * kPi));
        const _float x1 = sinf((t + p1) * f * (2.f * kPi) * 1.73f);
        const _float x2 = sinf((t + p2) * f * (2.f * kPi) * 2.31f);
        return x0 * 0.56f + x1 * 0.30f + x2 * 0.14f;
    }

    _float ClampAttack(_float dur)
    {
        const _float a = dur * 0.18f;
        return min(0.045f, max(0.010f, a));
    }
}

const CamShakePreset& ShakeController::GetPreset(CamShakeType type) const
{
    return kPresets[(int)type];
}

void ShakeController::Reset()
{
    m_instances.clear();
    m_seed = 1u;
}

void ShakeController::Set(_float ampDeg, _float freq, _float dur, _float fadeOutSec)
{
    m_instances.clear();
    Add(ampDeg, freq, dur, fadeOutSec);
}

void ShakeController::Add(_float ampDeg, _float freq, _float dur, _float fadeOutSec)
{
    CamShakePreset p{};
    p.kickRotDeg = ampDeg;
    p.kickPos = ampDeg * 0.0022f;
    p.kickDur = min(0.070f, max(0.035f, dur * 0.32f));

    p.noiseRotDeg = ampDeg * 0.55f;
    p.noisePos = ampDeg * 0.0011f;
    p.noiseFreq = max(8.f, freq);

    p.dur = max(0.030f, dur);
    p.fadeOutSec = max(0.f, fadeOutSec);

    AddPreset(p, 1.f);
}

void ShakeController::Set(CamShakeType type, _float strength)
{
    m_instances.clear();
    Add(type, strength);
}

void ShakeController::Add(CamShakeType type, _float strength)
{
    AddPreset(GetPreset(type), strength);
}

void ShakeController::AddPreset(const CamShakePreset& p, _float strength)
{
    CamShakeInstance s{};

    s.seed = HashU(m_seed++);
    s.elapsed = 0.f;

    s.attackSec = ClampAttack(p.dur);
    s.sustainSec = max(0.f, p.dur);
    s.decaySec = max(0.f, p.fadeOutSec);

    s.kickDur = p.kickDur;
    s.kickRotRad = DegToRad(p.kickRotDeg * strength);
    s.kickPos = p.kickPos * strength;

    s.noiseRotRad = DegToRad(p.noiseRotDeg * strength);
    s.noisePos = p.noisePos * strength;
    s.noiseFreq = p.noiseFreq * (0.92f + 0.18f * Hash01(s.seed + 11u));

    s.yawSign = (Hash01(s.seed + 21u) < 0.5f) ? -1.f : 1.f;
    s.rollSign = (Hash01(s.seed + 31u) < 0.5f) ? -1.f : 1.f;

    const _float side = Hash01(s.seed + 41u) * 2.f - 1.f;
    s.sideSign = (side < 0.f) ? -1.f : 1.f;

    s.p0 = Hash01(s.seed + 51u) * 10.f;
    s.p1 = Hash01(s.seed + 61u) * 10.f;
    s.p2 = Hash01(s.seed + 71u) * 10.f;

    m_instances.push_back(s);

    if (m_instances.size() > 6)
        m_instances.erase(m_instances.begin());
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

    _float pitchAcc = 0.f;
    _float yawAcc = 0.f;
    _float rollAcc = 0.f;

    Vector3 posAcc = Vector3::Zero;

    for (auto& s : m_instances)
    {
        const _float t = s.elapsed;
        s.elapsed += dt;

        const _float w = Envelope(t, s.attackSec, s.sustainSec, s.decaySec);
        if (w <= 0.f) continue;

        const _float k = KickCurve(t, s.kickDur);
        const _float n = Noise3(t, s.noiseFreq, s.p0, s.p1, s.p2);

        const _float kick = s.kickRotRad * w * k;
        const _float noise = s.noiseRotRad * w * n;

        pitchAcc += kick * 1.00f + noise * 0.55f;
        yawAcc += (kick * 0.22f + noise * 0.75f) * s.yawSign * s.sideSign;
        rollAcc += (kick * 0.18f + noise * 0.60f) * s.rollSign * s.sideSign;

        posAcc.z -= s.kickPos * w * k;
        posAcc.x += s.kickPos * w * k * 0.55f * (-s.sideSign);

        posAcc.y += s.noisePos * w * n * 0.45f;
        posAcc.x += s.noisePos * w * n * 0.30f;
        posAcc.z += s.noisePos * w * n * 0.20f;
    }

    outRotDelta = Quaternion::CreateFromYawPitchRoll(yawAcc, pitchAcc, rollAcc);
    outRotDelta.Normalize();

    const Matrix R = Matrix::CreateFromQuaternion(camRot);
    const Vector3 right = Vector3(R._11, R._12, R._13);
    const Vector3 up = Vector3(R._21, R._22, R._23);
    const Vector3 forward = Vector3(R._31, R._32, R._33);

    outWorldPosDelta = right * posAcc.x + up * posAcc.y + forward * posAcc.z;

    m_instances.erase(remove_if(m_instances.begin(), m_instances.end(), [&](const CamShakeInstance& s)
        {
            const _float endT = s.attackSec + s.sustainSec + max(0.f, s.decaySec);
            return s.elapsed >= endT;
        }), m_instances.end());
}