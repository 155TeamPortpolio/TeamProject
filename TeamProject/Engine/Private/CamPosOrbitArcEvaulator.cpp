#include "Engine_Defines.h"
#include "CamPosOrbitArcEvaulator.h"

bool CCamPosOrbitArcEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    if (keys.empty()) return false;

    keyframes = &keys;
    return true;
}

_vector3 CCamPosOrbitArcEvaluator::Evaluate(_float time) const
{
    const size_t n = keyframes->size();
    if (n == 1) return (*keyframes)[0].pos;

    const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);
    const _uint segIdx = segment.segmentIdx;
    float u = segment.normalizedTime;

    if (segIdx + 1 >= n) return (*keyframes)[n - 1].pos;

    const _vector3 p0 = (*keyframes)[segIdx].pos;
    const _vector3 p1 = (*keyframes)[segIdx + 1].pos;

    if (!orbitDesc->enabled) return p0 + (p1 - p0) * u;

    _vector3 axis = orbitDesc->axis;
    if (axis.LengthSquared() <= 1e-10f) return p0 + (p1 - p0) * u;
    axis.Normalize();

    if (u < 0.f) u = 0.f;
    if (u > 1.f) u = 1.f;

    const _vector3 c = orbitDesc->center;

    const _vector3 v0 = p0 - c;
    const _vector3 v1 = p1 - c;

    const float h0 = v0.Dot(axis);
    const float h1 = v1.Dot(axis);

    const _vector3 v0p = v0 - axis * h0;
    const _vector3 v1p = v1 - axis * h1;

    const float r0 = v0p.Length();
    const float r1 = v1p.Length();

    if (r0 <= 1e-6f || r1 <= 1e-6f) return p0 + (p1 - p0) * u;

    _vector3 d0 = v0p; d0.Normalize();
    _vector3 d1 = v1p; d1.Normalize();

    float angle = 0.f;

    if (orbitDesc->angleMode == CamOrbitArcAngleMode::Force180)
    {
        angle = orbitDesc->clockwise ? -DirectX::XM_PI : DirectX::XM_PI;
    }
    else
    {
        angle = SignedAngleAroundAxis(d0, d1, axis);

        if (orbitDesc->angleMode == CamOrbitArcAngleMode::Longest)
        {
            if (angle >= 0.f) angle -= DirectX::XM_2PI;
            else angle += DirectX::XM_2PI;
        }

        if (orbitDesc->clockwise)
        {
            if (angle > 0.f) angle -= DirectX::XM_2PI;
        }
        else
        {
            if (angle < 0.f) angle += DirectX::XM_2PI;
        }
    }

    const _vector3 dir = RotateAroundAxis(d0, axis, angle * u);

    float r = r0;
    if (orbitDesc->radiusMode == CamOrbitArcRadiusMode::FixedEndRadius) r = r1;
    else if (orbitDesc->radiusMode == CamOrbitArcRadiusMode::BlendRadius) r = r0 + (r1 - r0) * u;

    const float h = h0 + (h1 - h0) * u;

    return c + dir * r + axis * h;
}

_vector3 CCamPosOrbitArcEvaluator::NormalizeSafe(const _vector3& v, float eps)
{
    const float lsq = v.LengthSquared();
    if (lsq <= eps * eps) return _vector3(0.f, 0.f, 0.f);

    _vector3 out = v;
    out.Normalize();
    return out;
}

_vector3 CCamPosOrbitArcEvaluator::ProjectToPlane(const _vector3& v, const _vector3& unitAxis)
{
    const float d = v.Dot(unitAxis);
    return v - unitAxis * d;
}

_vector3 CCamPosOrbitArcEvaluator::RotateAroundAxis(const _vector3& v, const _vector3& unitAxis, float angleRad)
{
    const float c = cosf(angleRad);
    const float s = sinf(angleRad);

    const _vector3 cross = unitAxis.Cross(v);
    const float dot = unitAxis.Dot(v);

    return v * c + cross * s + unitAxis * (dot * (1.f - c));
}

float CCamPosOrbitArcEvaluator::SignedAngleAroundAxis(const _vector3& fromUnit, const _vector3& toUnit, const _vector3& unitAxis)
{
    const float cosv = clamp(fromUnit.Dot(toUnit), -1.f, 1.f);
    const _vector3 cr = fromUnit.Cross(toUnit);
    const float sinv = unitAxis.Dot(cr);
    return atan2f(sinv, cosv);
}