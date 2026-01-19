#include "Engine_Defines.h"
#include "CamFovPerSegmentEvaluator.h"
#include "CamFovLinearEvaluator.h"
#include "CamFovSmoothEvaluator.h"

bool CCamFovPerSegmentEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    keyframes = &keys;

    Safe_Release(evalLinear);
    Safe_Release(evalSmooth);

    evalLinear = CCamFovLinearEvaluator::Create();
    evalSmooth = CCamFovSmoothEvaluator::Create();

    bool ok = true;
    ok &= evalLinear->Build(keys);
    ok &= evalSmooth->Build(keys);

    return ok;
}

CamFovInterp CCamFovPerSegmentEvaluator::ResolveMode(_uint segIdx) const
{
    const auto& keys = *keyframes;

    if (keys.size() < 2) return seq->fovInterp;
    if (segIdx >= (_uint)keys.size() - 1) return seq->fovInterp;

    const CamKeyFrame& k = keys[(size_t)segIdx];
    if (k.useCustomInterp) return k.outFovInterp;

    return seq->fovInterp;
}

_float CCamFovPerSegmentEvaluator::Evaluate(_float time) const
{
    const auto& keys = *keyframes;
    const size_t n = keys.size();

    if (n == 0) return 60.f;
    if (n == 1) return keys[0].fov;

    if (time <= keys[0].time) return keys[0].fov;
    if (time >= keys.back().time) return keys.back().fov;

    const CamKeySegment segment = CamUtil::FindKeySegment(keys, time);
    const _uint i = segment.segmentIdx;

    if ((size_t)i + 1 >= n) return keys.back().fov;

    const float t0 = keys[(size_t)i].time;
    const float t1 = keys[(size_t)i + 1].time;

    const float eps = 1e-6f;
    if (fabsf(time - t0) <= eps) return keys[(size_t)i].fov;
    if (fabsf(time - t1) <= eps) return keys[(size_t)i + 1].fov;

    const CamFovInterp mode = ResolveMode(i);

    switch (mode)
    {
    case CamFovInterp::Linear:
        return evalLinear ? evalLinear->Evaluate(time) : keys[(size_t)i].fov;
    case CamFovInterp::Smooth:
        return evalSmooth ? evalSmooth->Evaluate(time) : (evalLinear ? evalLinear->Evaluate(time) : keys[(size_t)i].fov);
    }
    return evalLinear ? evalLinear->Evaluate(time) : keys[(size_t)i].fov;
}

void CCamFovPerSegmentEvaluator::Free()
{
    Safe_Release(evalLinear);
    Safe_Release(evalSmooth);
    __super::Free();
}