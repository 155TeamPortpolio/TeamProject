#include "Engine_Defines.h"
#include "CamPosCentripetalEvaluator.h"

bool CCamPosCentripetalEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    if (keys.empty())
        return false;

    keyframes = &keys;
    alpha = 0.5f;
    return true;
}

_vector3 CCamPosCentripetalEvaluator::GetPosExtended(int idx) const
{
    assert(keyframes);
    const int n = (int)keyframes->size();
    assert(n >= 1);

    if (idx < 0)
    {
        if (n == 1) return (*keyframes)[0].pos;
        const _vector3 p1 = (*keyframes)[0].pos;
        const _vector3 p2 = (*keyframes)[1].pos;
        return p1 + (p1 - p2);
    }

    if (idx >= n)
    {
        if (n == 1) return (*keyframes)[0].pos;
        const _vector3 pnm1 = (*keyframes)[n - 2].pos;
        const _vector3 pn = (*keyframes)[n - 1].pos;
        return pn + (pn - pnm1);
    }

    return (*keyframes)[(size_t)idx].pos;
}

_vector3 CCamPosCentripetalEvaluator::Evaluate(_float time) const
{
    assert(keyframes);
    assert(!keyframes->empty());

    const size_t n = keyframes->size();

    if (n == 1)
        return (*keyframes)[0].pos;

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

    const _vector3 p0 = GetPosExtended(i1 - 1);
    const _vector3 p1 = GetPosExtended(i1);
    const _vector3 p2 = GetPosExtended(i2);
    const _vector3 p3 = GetPosExtended(i2 + 1);

    auto DistPow = [&](const _vector3& a, const _vector3& b) -> float
        {
            const float d = (b - a).Length();
            if (d <= 1e-6f) return 0.f;
            return powf(d, alpha);
        };

    float t0 = 0.f;
    float t1 = t0 + DistPow(p0, p1);
    float t2 = t1 + DistPow(p1, p2);
    float t3 = t2 + DistPow(p2, p3);

    if ((t2 - t1) <= 1e-6f)
    {
        const float u = segment.normalizedTime;
        return p1 + (p2 - p1) * u;
    }

    const float u = segment.normalizedTime;
    const float t = t1 + (t2 - t1) * u;

    auto Lerp = [&](const _vector3& a, const _vector3& b, float s) -> _vector3
        {
            s = std::clamp(s, 0.f, 1.f);
            return a + (b - a) * s;
        };

    auto SafeRatio = [&](float a, float b) -> float
        {
            if (fabsf(b) <= 1e-6f) return 0.f;
            return a / b;
        };

    const _vector3 A1 = Lerp(p0, p1, SafeRatio(t - t0, t1 - t0));
    const _vector3 A2 = Lerp(p1, p2, SafeRatio(t - t1, t2 - t1));
    const _vector3 A3 = Lerp(p2, p3, SafeRatio(t - t2, t3 - t2));

    const _vector3 B1 = Lerp(A1, A2, SafeRatio(t - t0, t2 - t0));
    const _vector3 B2 = Lerp(A2, A3, SafeRatio(t - t1, t3 - t1));

    const _vector3 C = Lerp(B1, B2, SafeRatio(t - t1, t2 - t1));
    return C;
}
