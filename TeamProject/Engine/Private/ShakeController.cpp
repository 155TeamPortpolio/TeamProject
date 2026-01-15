#include "Engine_Defines.h"
#include "ShakeController.h"
#include "Engine_Math.h"

namespace
{
    _float Clamp01Local(_float t)
    {
        if (t < 0.f) return 0.f;
        if (t > 1.f) return 1.f;
        return t;
    }
}

_uint ShakeController::HashU(_uint x)
{
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

_float ShakeController::Hash01(_uint x)
{
    return (HashU(x) & 0x00FFFFFF) / (float)0x01000000;
}

_float ShakeController::Smooth01(_float t)
{
    t = Clamp01Local(t);
    return t * t * (3.f - 2.f * t);
}

_float ShakeController::EnvelopeEased(_float elapsed, _float attackSec, _float sustainSec, _float decaySec)
{
    if (elapsed < attackSec) return attackSec <= 0.f ? 1.f : Smooth01(elapsed / attackSec);

    elapsed -= attackSec;
    if (elapsed < sustainSec) return 1.f;

    elapsed -= sustainSec;
    if (decaySec <= 0.f) return 0.f;

    return 1.f - Smooth01(elapsed / decaySec);
}

_float ShakeController::KickPulse(_float t, _float attackSec, _float durationSec, _float decaySec)
{
    if (durationSec <= 0.f) return 0.f;
    if (t >= durationSec) return 0.f;

    _float a = 1.f;
    if (t < attackSec) a = attackSec <= 0.f ? 1.f : Smooth01(t / attackSec);

    const _float u = t / durationSec;
    const _float pulse = sinf(u * XM_PI);

    const _float d = decaySec <= 0.f ? 1.f : expf(-t / decaySec);

    return a * pulse * d;
}

_float ShakeController::ClampAttack(_float sustainSec)
{
    const _float a = sustainSec * 0.25f;
    return min(0.04f, max(0.01f, a));
}

ShakeController::PresetKick ShakeController::GetPreset(CamShakeType type)
{
    switch (type)
    {
    case CamShakeType::HitNormal:
        return PresetKick{0.85f, 0.0016f, 0.030f, 0.060f, 0.075f};

    case CamShakeType::HitHeavy:
        return PresetKick{1.90f, 0.0038f, 0.040f, 0.090f, 0.110f};
    }

    return {};
}

void ShakeController::Reset()
{
    m_instances.clear();
    m_seed = 1u;
}

void ShakeController::Set(_float ampDeg, _float sustainSec, _float fadeOutSec)
{
    m_instances.clear();
    Add(ampDeg, sustainSec, fadeOutSec);
}

void ShakeController::Add(_float ampDeg, _float sustainSec, _float fadeOutSec)
{
    PresetKick p{};
    p.rotAmpDeg = ampDeg;
    p.posAmp = ampDeg * 0.0018f;
    p.sustainSec = sustainSec;
    p.fadeOutSec = fadeOutSec;
    p.kickDurationSec = max(0.05f, sustainSec + fadeOutSec);
    AddKick(p, 1.f);
}

void ShakeController::Set(CamShakeType type, _float strength)
{
    m_instances.clear();
    Add(type, strength);
}

void ShakeController::Add(CamShakeType type, _float strength)
{
    AddKick(GetPreset(type), strength);
}

void ShakeController::AddKick(const PresetKick& p, _float strength)
{
    Instance s{};
    s.rotAmpDeg = p.rotAmpDeg * strength;
    s.posAmp = p.posAmp * strength;

    s.attackSec = ClampAttack(p.sustainSec);
    s.sustainSec = p.sustainSec;
    s.decaySec = p.fadeOutSec;
    s.elapsed = 0.f;

    s.kickDurationSec = p.kickDurationSec;
    s.kickDecaySec = max(0.035f, p.kickDurationSec * 0.45f);

    const _uint seed = m_seed++;
    s.yawSign = Hash01(seed ^ 0xA2F1D3B5u) < 0.5f ? -1.f : 1.f;
    s.rollSign = Hash01(seed ^ 0xC1B9E6A7u) < 0.5f ? -1.f : 1.f;

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

    _float pitchAcc = 0.f;
    _float yawAcc = 0.f;
    _float rollAcc = 0.f;

    Vector3 posAcc = Vector3::Zero;

    for (auto& s : m_instances)
    {
        s.elapsed += dt;

        const _float w = EnvelopeEased(s.elapsed, s.attackSec, s.sustainSec, s.decaySec);
        if (w <= 0.f) continue;

        const _float kw = KickPulse(s.elapsed, 0.012f, s.kickDurationSec, s.kickDecaySec);
        if (kw <= 0.f) continue;

        const _float kickRad = XMConvertToRadians(s.rotAmpDeg) * w * kw;

        pitchAcc += kickRad;
        yawAcc += kickRad * 0.10f * s.yawSign;
        rollAcc += kickRad * 0.04f * s.rollSign;

        posAcc.z -= s.posAmp * w * kw;
        posAcc.y += s.posAmp * w * kw * 0.35f;
    }

    outRotDelta = Quaternion::CreateFromYawPitchRoll(yawAcc * 0.6f, pitchAcc, rollAcc * 0.4f);
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