#include "Engine_Defines.h"
#include "CamFovSmoothEvaluator.h"

bool CamFovSmoothEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    if (keys.empty())
        return false;

    keyframes = &keys;
    return true;
}

_float CamFovSmoothEvaluator::Evaluate(_float time) const
{
    assert(keyframes);
    assert(!keyframes->empty());

    if (keyframes->size() == 1)
        return (*keyframes)[0].fov;

    const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

    const _uint segmentIdx = segment.segmentIdx;
    float u = segment.normalizedTime;

    u = clamp(u, 0.f, 1.f);
    const float uSmooth = u * u * (3.f - 2.f * u);

    const float startFov = (*keyframes)[segmentIdx].fov;
    const float endFov = (*keyframes)[segmentIdx + 1].fov;

    return startFov + (endFov - startFov) * uSmooth;
}