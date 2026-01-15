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
    return a * sinf(u * XM_PI);
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
        return PresetKick{0.18f, 0.00010f, 0.015f, 0.035f, 0.045f};

    case CamShakeType::HitHeavy:
        return PresetKick{0.45f, 0.00022f, 0.020f, 0.050f, 0.060f};
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
    p.posAmp = ampDeg * 0.0009f;
    p.sustainSec = sustainSec;
    p.fadeOutSec = fadeOutSec;
    p.kickDurationSec = min(0.10f, max(0.05f, sustainSec));
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
    const _float addRot = p.rotAmpDeg * strength;
    const _float addPos = p.posAmp * strength;

    if (m_instances.empty())
    {
        Instance s{};
        s.rotAmpDeg = addRot;
        s.posAmp = addPos;

        s.attackSec = ClampAttack(p.sustainSec);
        s.sustainSec = p.sustainSec;
        s.decaySec = p.fadeOutSec;
        s.elapsed = 0.f;

        s.kickDurationSec = p.kickDurationSec;

        s.yawSign = 1.f;
        s.rollSign = 1.f;

        m_instances.push_back(s);
        return;
    }

    auto& s = m_instances.front();

    s.rotAmpDeg = min(1.2f, s.rotAmpDeg + addRot);
    s.posAmp = min(0.0020f, s.posAmp + addPos);

    s.attackSec = min(s.attackSec, ClampAttack(p.sustainSec));
    s.sustainSec = max(s.sustainSec, p.sustainSec);
    s.decaySec = max(s.decaySec, p.fadeOutSec);
    s.kickDurationSec = min(0.10f, max(s.kickDurationSec, p.kickDurationSec));

    s.elapsed = 0.f;
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

        const _float kw = KickPulse(s.elapsed, 0.010f, s.kickDurationSec, 0.f);
        if (kw <= 0.f) continue;

        const _float kickRad = XMConvertToRadians(s.rotAmpDeg) * w * kw;

        pitchAcc += kickRad;
        yawAcc += kickRad * 0.05f;
        rollAcc += kickRad * 0.015f;

        posAcc.y += s.posAmp * w * kw * 0.06f;
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
