#include "Engine_Defines.h"
#include "CamRotSquadEvaluator.h"

bool CCamRotSquadEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    if (keys.empty()) return false;

    keyframes = &keys;

    cachedRots.clear();
    cachedRots.resize(keys.size());

    for (size_t keyIdx = 0; keyIdx < keys.size(); ++keyIdx)
    {
        const CamKeyFrame& key = keys[keyIdx];

        const float lookLengthSq = key.look.LengthSquared();
        if (lookLengthSq < 1e-8f) return false;

        cachedRots[keyIdx] = MakeRotFromLookRoll(key.look, key.roll);
        cachedRots[keyIdx] = QNormalizeSafe(cachedRots[keyIdx]);
    }
    for (size_t keyIdx = 1; keyIdx < cachedRots.size(); ++keyIdx)
    {
        const float dotValue = cachedRots[keyIdx - 1].Dot(cachedRots[keyIdx]);
        if (dotValue < 0.f) cachedRots[keyIdx] = -cachedRots[keyIdx];
    }

    cachedTans.clear();
    cachedTans.resize(keys.size());

    for (size_t i = 0; i < cachedRots.size(); ++i)
    {
        cachedTans[i] = ComputeTangent((int)i);

        const float d = cachedRots[i].Dot(cachedTans[i]);
        if (d < 0.f) cachedTans[i] = -cachedTans[i];

        cachedTans[i] = QNormalizeSafe(cachedTans[i]);
    }
    for (size_t i = 1; i < cachedTans.size(); ++i)
    {
        if (cachedTans[i - 1].Dot(cachedTans[i]) < 0.f) cachedTans[i] = -cachedTans[i];
        cachedTans[i] = QNormalizeSafe(cachedTans[i]);
    }

    return true;
}

Quaternion CCamRotSquadEvaluator::Evaluate(_float time) const
{
    if (keyframes->size() == 1) return cachedRots[0];

    const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

    const _uint segmentIdx = segment.segmentIdx;
    const float u = segment.normalizedTime;

    const Quaternion& q0 = cachedRots[segmentIdx];
    const Quaternion& q1 = cachedRots[segmentIdx + 1];

    if (keyframes->size() == 2)
    {
        Quaternion out = Quaternion::Slerp(q0, q1, u);
        if (out.Dot(q0) < 0.f) out = -out;
        out.Normalize();
        return out;
    }

    Quaternion s0 = cachedTans[segmentIdx];
    Quaternion s1 = cachedTans[segmentIdx + 1];

    if (s0.Dot(q0) < 0.f) s0 = -s0;
    if (s1.Dot(q1) < 0.f) s1 = -s1;
    if (s0.Dot(s1) < 0.f) s1 = -s1;

    Quaternion a = Quaternion::Slerp(q0, q1, u);
    Quaternion b = Quaternion::Slerp(s0, s1, u);

    if (b.Dot(a) < 0.f) b = -b;

    const float k = 2.f * u * (1.f - u);
    Quaternion out = Quaternion::Slerp(a, b, k);

    if (out.Dot(q0) < 0.f) out = -out;

    out.Normalize();
    return out;
}

Quaternion CCamRotSquadEvaluator::MakeRotFromLookRoll(_vector3 look, _float rollRad) const
{
    if (look.LengthSquared() <= 1e-8f)
        look = _vector3(0.f, 0.f, 1.f);
    else
        look.Normalize();

    const _vector3 forward = look;

    _vector3 referenceUp(0.f, 1.f, 0.f);
    const float parallel = fabsf(forward.Dot(referenceUp));
    if (parallel > 0.999f)
        referenceUp = _vector3(0.f, 0.f, 1.f);

    _vector3 right = referenceUp.Cross(forward);
    right.Normalize();

    _vector3 up = forward.Cross(right);
    up.Normalize();

    Matrix basis;
    basis._11 = right.x;   basis._12 = right.y;   basis._13 = right.z;   basis._14 = 0.f;
    basis._21 = up.x;      basis._22 = up.y;      basis._23 = up.z;      basis._24 = 0.f;
    basis._31 = forward.x; basis._32 = forward.y; basis._33 = forward.z; basis._34 = 0.f;
    basis._41 = 0.f;       basis._42 = 0.f;       basis._43 = 0.f;       basis._44 = 1.f;

    Quaternion baseRot = Quaternion::CreateFromRotationMatrix(basis);
    Quaternion rollRot = Quaternion::CreateFromAxisAngle(forward, rollRad);

    Quaternion finalRot = rollRot * baseRot;
    finalRot.Normalize();
    return finalRot;
}

Quaternion CCamRotSquadEvaluator::GetRotClamped(int idx) const
{
    const int n = (int)cachedRots.size();

    if (idx < 0) idx = 0;
    if (idx >= n) idx = n - 1;

    return cachedRots[(size_t)idx];
}

Quaternion CCamRotSquadEvaluator::GetTanClamped(int idx) const
{
    const int n = (int)cachedTans.size();

    if (idx < 0) idx = 0;
    if (idx >= n) idx = n - 1;

    return cachedTans[(size_t)idx];
}

Quaternion CCamRotSquadEvaluator::ComputeTangent(int idx) const
{
    const int n = (int)cachedRots.size();
    const Quaternion qCur = cachedRots[(size_t)idx];

    if (idx <= 0 || idx >= n - 1)
        return qCur;

    Quaternion qPrev = cachedRots[(size_t)(idx - 1)];
    Quaternion qNext = cachedRots[(size_t)(idx + 1)];

    if (qCur.Dot(qPrev) < 0.f) qPrev = -qPrev;
    if (qCur.Dot(qNext) < 0.f) qNext = -qNext;

    const float dp = qCur.Dot(qPrev);
    const float dn = qCur.Dot(qNext);

    if (dp < 0.2f || dn < 0.2f)
        return qCur;

    const Quaternion qInv = QConjugate(qCur);

    const Quaternion a = QLogUnit(QMul(qInv, qPrev));
    const Quaternion b = QLogUnit(QMul(qInv, qNext));

    Quaternion sum{};
    sum.x = (a.x + b.x) * -0.25f;
    sum.y = (a.y + b.y) * -0.25f;
    sum.z = (a.z + b.z) * -0.25f;
    sum.w = 0.f;

    const Quaternion expSum = QExpPure(sum);
    Quaternion out = QMul(qCur, expSum);

    if (out.Dot(qCur) < 0.f) out = -out;
    out = QNormalizeSafe(out);
    return out;
}

Quaternion CCamRotSquadEvaluator::QLogUnit(const Quaternion& q)
{
    const float w = clamp(q.w, -1.f, 1.f);
    const float vLenSq = q.x * q.x + q.y * q.y + q.z * q.z;

    Quaternion out{};
    out.w = 0.f;

    if (vLenSq <= 1e-12f)
    {
        out.x = 0.f;
        out.y = 0.f;
        out.z = 0.f;
        return out;
    }

    const float vLen = sqrtf(vLenSq);
    const float angle = acosf(w);
    const float s = angle / vLen;

    out.x = q.x * s;
    out.y = q.y * s;
    out.z = q.z * s;
    return out;
}

Quaternion CCamRotSquadEvaluator::QExpPure(const Quaternion& q)
{
    const float vLenSq = q.x * q.x + q.y * q.y + q.z * q.z;

    Quaternion out{};

    if (vLenSq <= 1e-12f)
    {
        out.x = q.x;
        out.y = q.y;
        out.z = q.z;
        out.w = 1.f;
        return out;
    }

    const float vLen = sqrtf(vLenSq);
    const float sinA = sinf(vLen);
    const float cosA = cosf(vLen);
    const float s = sinA / vLen;

    out.x = q.x * s;
    out.y = q.y * s;
    out.z = q.z * s;
    out.w = cosA;
    return out;
}

Quaternion CCamRotSquadEvaluator::QMul(const Quaternion& a, const Quaternion& b)
{
    Quaternion out{};
    out.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    out.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
    out.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
    out.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    return out;
}

Quaternion CCamRotSquadEvaluator::QConjugate(const Quaternion& q)
{
    Quaternion out{};
    out.x = -q.x;
    out.y = -q.y;
    out.z = -q.z;
    out.w = q.w;
    return out;
}

Quaternion CCamRotSquadEvaluator::QNormalizeSafe(const Quaternion& q)
{
    Quaternion out = q;

    const float lenSq = out.x * out.x + out.y * out.y + out.z * out.z + out.w * out.w;
    if (lenSq <= 1e-12f)
    {
        out.x = 0.f;
        out.y = 0.f;
        out.z = 0.f;
        out.w = 1.f;
        return out;
    }

    const float invLen = 1.f / sqrtf(lenSq);
    out.x *= invLen;
    out.y *= invLen;
    out.z *= invLen;
    out.w *= invLen;
    return out;
}