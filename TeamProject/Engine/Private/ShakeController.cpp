#include "Engine_Defines.h"
#include "ShakeController.h"
#include "Engine_Math.h"

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
    t = Math::Clamp01(t);
    return t * t * (3.f - 2.f * t);
}

_float ShakeController::ValueNoise1D(_uint seed, _float x)
{
    const _int xi = (_int)floorf(x);
    const _float t = x - (_float)xi;

    const _float a = Hash01(seed ^ (_uint)xi) * 2.f - 1.f;
    const _float b = Hash01(seed ^ (_uint)(xi + 1)) * 2.f - 1.f;

    return a + (b - a) * Smooth01(t);
}

Vector3 ShakeController::Noise3(_uint seed, _float t)
{
    return {
        ValueNoise1D(seed + 11u, t),
        ValueNoise1D(seed + 23u, t),
        ValueNoise1D(seed + 37u, t)
    };
}

Vector3 ShakeController::FBM3(_uint seed, _float t)
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

_float ShakeController::Envelope(_float elapsed, _float attackSec, _float sustainSec, _float decaySec)
{
    if (elapsed < attackSec) return attackSec <= 0.f ? 1.f : Math::Clamp01(elapsed / attackSec);

    elapsed -= attackSec;
    if (elapsed < sustainSec) return 1.f;

    elapsed -= sustainSec;
    if (decaySec <= 0.f) return 0.f;
    return 1.f - Math::Clamp01(elapsed / decaySec);
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
    Instance s{};
    s.amplitudeDeg = ampDeg;
    s.frequency = freq;
    s.attackSec = min(0.03f, sustainSec * 0.25f);
    s.sustainSec = sustainSec;
    s.decaySec = fadeOutSec;
    s.elapsed = 0.f;
    s.posAmp = ampDeg * 0.0025f;
    s.seed = m_seed++;

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

    Vector3 rotAcc{};
    Vector3 posAcc{};

    for (auto& s : m_instances)
    {
        s.elapsed += dt;

        const _float w = Envelope(s.elapsed, s.attackSec, s.sustainSec, s.decaySec);
        if (w <= 0.f) continue;

        const _float t = s.elapsed * s.frequency;
        Vector3 n = FBM3(s.seed, t);

        const _float rotAmpRad = DirectX::XMConvertToRadians(s.amplitudeDeg) * w;
        rotAcc += n * rotAmpRad;

        const _float posAmp = s.posAmp * w;
        posAcc += n * posAmp;
    }

    outRotDelta = Quaternion::CreateFromYawPitchRoll(rotAcc.y * 0.7f, rotAcc.x, rotAcc.z * 0.6f);
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