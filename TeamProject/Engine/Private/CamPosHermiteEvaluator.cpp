#include "Engine_Defines.h"
#include "CamPosHermiteEvaluator.h"

bool CCamPosHermiteEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    if (keys.empty()) return false;

    keyframes = &keys;
    tension = clamp(tension, 0.f, 1.f);
    return true;
}

_vector3 CCamPosHermiteEvaluator::Evaluate(_float time) const
{
    assert(keyframes);
    assert(!keyframes->empty());

    const size_t n = keyframes->size();

    if (n == 1) return (*keyframes)[0].pos;

    if (n == 2)
    {
        const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);
        const _uint i = segment.segmentIdx;
        const float u = segment.normalizedTime;

        const _vector3& a = (*keyframes)[i].pos;
        const _vector3& b = (*keyframes)[i + 1].pos;
        return a + (b - a) * u;
    }

    const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

    const int i1 = (int)segment.segmentIdx;
    const int i2 = i1 + 1;

    const _vector3 p0 = GetPosClamped(i1 - 1);
    const _vector3 p1 = GetPosClamped(i1);
    const _vector3 p2 = GetPosClamped(i2);
    const _vector3 p3 = GetPosClamped(i2 + 1);

    const float s = 0.5f * (1.f - (float)tension);

    const _vector3 m1 = (p2 - p0) * s;
    const _vector3 m2 = (p3 - p1) * s;

    const float u = segment.normalizedTime;
    return Hermite(p1, p2, m1, m2, u);
}

_vector3 CCamPosHermiteEvaluator::GetPosClamped(int idx) const
{
    assert(keyframes);

    const int n = (int)keyframes->size();

    if (idx < 0) idx = 0;
    if (idx >= n) idx = n - 1;

    return (*keyframes)[(size_t)idx].pos;
}

_vector3 CCamPosHermiteEvaluator::Hermite(const _vector3& p1, const _vector3& p2, const _vector3& m1, const _vector3& m2, float u) const
{
    const float u2 = u * u;
    const float u3 = u2 * u;

    const float h00 = 2.f * u3 - 3.f * u2 + 1.f;
    const float h10 = u3 - 2.f * u2 + u;
    const float h01 = -2.f * u3 + 3.f * u2;
    const float h11 = u3 - u2;

    return p1 * h00 + m1 * h10 + p2 * h01 + m2 * h11;
}