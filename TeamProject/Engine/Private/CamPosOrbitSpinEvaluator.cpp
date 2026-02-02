#include "Engine_Defines.h"
#include "Helper_Func.h"
#include "CamPosOrbitSpinEvaluator.h"

namespace
{
    float SignedAngleOnAxis(const Vector3& aDir, const Vector3& bDir, const Vector3& axisN)
    {
        const Vector3 c = aDir.Cross(bDir);
        const float s = axisN.Dot(c);
        const float d = clamp(aDir.Dot(bDir), -1.f, 1.f);
        return atan2f(s, d);
    }

    Vector3 SafeNormalizeOrX(const Vector3& v)
    {
        const float lenSq = v.LengthSquared();
        if (lenSq <= 1e-12f) return Vector3(1.f, 0.f, 0.f);
        Vector3 x = v;
        x.Normalize();
        return x;
    }
}

Vector3 CCamPosOrbitSpinEvaluator::ResolveCenter() const
{
    if (desc->centerMode == CamSpinCenterMode::Custom)
        return Vector3(desc->center.x, desc->center.y, desc->center.z);

    return Vector3::Zero;
}

Vector3 CCamPosOrbitSpinEvaluator::ResolveAxisN(_bool keepHeight) const
{
    if (keepHeight)
        return Vector3(0.f, 1.f, 0.f);

    Vector3 a(desc->axis.x, desc->axis.y, desc->axis.z);
    if (a.LengthSquared() <= 1e-12f) a = Vector3(0.f, 1.f, 0.f);
    a.Normalize();
    return a;
}

void CCamPosOrbitSpinEvaluator::BuildSegmentCache(const CamKeyFrame& k0, const CamKeyFrame& k1, SegmentCache& out) const
{
    out.keepHeight = desc->keepHeight;

    out.center = ResolveCenter();
    out.axisN = ResolveAxisN(out.keepHeight);

    const Vector3 p0(k0.pos.x, k0.pos.y, k0.pos.z);
    const Vector3 p1(k1.pos.x, k1.pos.y, k1.pos.z);

    Vector3 a = p0 - out.center;
    Vector3 b = p1 - out.center;

    if (out.keepHeight)
    {
        out.baseY = p0.y;

        a.y = 0.f;
        b.y = 0.f;

        out.aPar = Vector3::Zero;
        out.bPar = Vector3::Zero;

        out.aLen = a.Length();
        out.bLen = b.Length();

        const Vector3 aDir = SafeNormalizeOrX(a);
        const Vector3 bDir = SafeNormalizeOrX(b);

        float ang = SignedAngleOnAxis(aDir, bDir, out.axisN);

        if (desc->clockwise && ang > 0.f) ang -= XM_2PI;
        if (!desc->clockwise && ang < 0.f) ang += XM_2PI;

        const float turnAdd = (desc->clockwise ? -XM_2PI : XM_2PI) * float(desc->extraTurns);
        out.totalAngle = ang + turnAdd;

        out.aDir = aDir;
        return;
    }

    const Vector3 aPar = out.axisN * a.Dot(out.axisN);
    const Vector3 bPar = out.axisN * b.Dot(out.axisN);

    Vector3 aPerp = a - aPar;
    Vector3 bPerp = b - bPar;

    out.aPar = aPar;
    out.bPar = bPar;

    out.aLen = aPerp.Length();
    out.bLen = bPerp.Length();

    const Vector3 aDir = SafeNormalizeOrX(aPerp);
    const Vector3 bDir = SafeNormalizeOrX(bPerp);

    float ang = SignedAngleOnAxis(aDir, bDir, out.axisN);

    if (desc->clockwise && ang > 0.f) ang -= XM_2PI;
    if (!desc->clockwise && ang < 0.f) ang += XM_2PI;

    const float turnAdd = (desc->clockwise ? -XM_2PI : XM_2PI) * float(desc->extraTurns);
    out.totalAngle = ang + turnAdd;

    out.aDir = aDir;
}

bool CCamPosOrbitSpinEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    assert(desc);

    keyframes = &keys;

    cache.clear();
    if (keys.size() < 2) return true;

    cache.resize(keys.size() - 1);

    for (size_t i = 0; i + 1 < keys.size(); ++i)
        BuildSegmentCache(keys[i], keys[i + 1], cache[i]);

    return true;
}

_vector3 CCamPosOrbitSpinEvaluator::Evaluate(_float time) const
{
    assert(desc);
    assert(desc->enabled);
    assert(keyframes);

    const auto& keys = *keyframes;
    const size_t n = keys.size();

    if (n == 0) return _vector3{};
    if (n == 1) return keys[0].pos;

    if (time <= keys[0].time) return keys[0].pos;
    if (time >= keys.back().time) return keys.back().pos;

    const CamKeySegment segment = CamUtil::FindKeySegment(keys, time);
    const _uint i = segment.segmentIdx;
    const float u = segment.normalizedTime;

    const SegmentCache& c = cache[(size_t)i];

    const float ang = c.totalAngle * u;
    const Quaternion q = Quaternion::CreateFromAxisAngle(c.axisN, ang);
    const Matrix rm = Matrix::CreateFromQuaternion(q);

    Vector3 dir = Vector3::Transform(c.aDir, rm);

    const float len = c.aLen + (c.bLen - c.aLen) * u;
    Vector3 perp = dir * len;

    Vector3 par = c.aPar + (c.bPar - c.aPar) * u;

    Vector3 p = c.center + par + perp;

    if (c.keepHeight) p.y = c.baseY;

    return _vector3(p.x, p.y, p.z);
}