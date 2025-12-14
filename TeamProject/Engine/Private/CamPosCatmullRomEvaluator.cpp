#include "Engine_Defines.h"
#include "CamPosCatmullRomEvaluator.h"
#include "CamPosCentripetalEvaluator.h"

bool CamPosCatmullRomEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    if (keys.empty())
        return false;

    keyframes = &keys;
    return true;
}

_vector3 CamPosCatmullRomEvaluator::Evaluate(_float time) const
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

    const _vector3 p0 = GetPosClamped(i1 - 1);
    const _vector3 p1 = GetPosClamped(i1);
    const _vector3 p2 = GetPosClamped(i2);
    const _vector3 p3 = GetPosClamped(i2 + 1);

    const float u = segment.normalizedTime;
    return CatmullRom(p0, p1, p2, p3, u);
}

_vector3 CamPosCatmullRomEvaluator::GetPosClamped(int idx) const
{
    assert(keyframes);
    const int n = (int)keyframes->size();

    if (idx < 0)  idx = 0;
    if (idx >= n) idx = n - 1;

    return (*keyframes)[(size_t)idx].pos;
}

_vector3 CamPosCatmullRomEvaluator::CatmullRom(const _vector3& p0, const _vector3& p1, const _vector3& p2, const _vector3& p3, float u) const
{
    const float u2 = u * u;
    const float u3 = u2 * u;

    const _vector3 a = p1 * 2.0f;
    const _vector3 b = (p2 - p0) * u;
    const _vector3 c = (p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * u2;
    const _vector3 d = (-p0 + p1 * 3.0f - p2 * 3.0f + p3) * u3;

    return (a + b + c + d) * 0.5f;
}