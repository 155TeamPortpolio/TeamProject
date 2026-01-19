#include "Engine_Defines.h"
#include "CamPosBSplineEvaluator.h"

bool CCamPosBSplineEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    if (keys.empty()) return false;

    keyframes = &keys;
    return true;
}

_vector3 CCamPosBSplineEvaluator::Evaluate(_float time) const
{
    const size_t n = keyframes->size();

    if (n == 1) return (*keyframes)[0].pos;

    if (n < 4)
    {
        const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

        const _uint i = segment.segmentIdx;
        const float u = segment.normalizedTime;

        const _vector3& a = (*keyframes)[i].pos;
        const _vector3& b = (*keyframes)[i + 1].pos;

        return a + (b - a) * u;
    }

    const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

    const int i = (int)segment.segmentIdx;
    const float u = segment.normalizedTime;

    const _vector3 p0 = GetPosClamped(i - 1);
    const _vector3 p1 = GetPosClamped(i);
    const _vector3 p2 = GetPosClamped(i + 1);
    const _vector3 p3 = GetPosClamped(i + 2);

    return BSplineCubicUniform(p0, p1, p2, p3, u);
}

_vector3 CCamPosBSplineEvaluator::GetPosClamped(int idx) const
{
    const int n = (int)keyframes->size();

    if (idx < 0) idx = 0;
    if (idx >= n) idx = n - 1;

    return (*keyframes)[(size_t)idx].pos;
}

_vector3 CCamPosBSplineEvaluator::BSplineCubicUniform(const _vector3& p0, const _vector3& p1, const _vector3& p2, const _vector3& p3, float u) const
{
    const float u2 = u * u;
    const float u3 = u2 * u;

    const float b0 = (1.f - 3.f * u + 3.f * u2 - u3) / 6.f;
    const float b1 = (4.f - 6.f * u2 + 3.f * u3) / 6.f;
    const float b2 = (1.f + 3.f * u + 3.f * u2 - 3.f * u3) / 6.f;
    const float b3 = (u3) / 6.f;

    return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}