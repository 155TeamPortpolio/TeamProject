#include "Engine_Defines.h"
#include "CamPosPerSegmentEvaluator.h"
#include "CamPosLinearEvaluator.h"
#include "CamPosCatmullRomEvaluator.h"
#include "CamPosCentripetalEvaluator.h"
#include "CamPosBSplineEvaluator.h"
#include "CamPosHermiteEvaluator.h"

bool CCamPosPerSegmentEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    assert(seq);

    keyframes = &keys;

    Safe_Release(evalLinear);
    Safe_Release(evalCatmull);
    Safe_Release(evalCentripetal);
    Safe_Release(evalBSpline);
    Safe_Release(evalHermite);

    evalLinear      = CCamPosLinearEvaluator::Create();
    evalCatmull     = CCamPosCatmullRomEvaluator::Create();
    evalCentripetal = CCamPosCentripetalEvaluator::Create();
    evalBSpline     = CCamPosBSplineEvaluator::Create();
    evalHermite     = CCamPosHermiteEvaluator::Create();

    bool ok = true;
    ok &= evalLinear->Build(keys);
    ok &= evalCatmull->Build(keys);
    ok &= evalCentripetal->Build(keys);
    ok &= evalBSpline->Build(keys);
    ok &= evalHermite->Build(keys);

    return ok;
}

_vector3 CCamPosPerSegmentEvaluator::Evaluate(_float time) const
{
    assert(seq);
    assert(keyframes);

    const auto& keys = *keyframes;
    const size_t n = keys.size();

    if (n == 0) return _vector3{};
    if (n == 1) return keys[0].pos;

    if (time <= keys[0].time) return keys[0].pos;
    if (time >= keys.back().time) return keys.back().pos;

    const CamKeySegment segment = CamUtil::FindKeySegment(keys, time);
    const _uint i = segment.segmentIdx;

    if ((size_t)i + 1 >= n) return keys.back().pos;

    const float t0 = keys[(size_t)i].time;
    const float t1 = keys[(size_t)i + 1].time;

    const float eps = 1e-6f;
    if (fabsf(time - t0) <= eps) return keys[(size_t)i].pos;
    if (fabsf(time - t1) <= eps) return keys[(size_t)i + 1].pos;

    const CamPosInterp mode = ResolveMode(i);

    switch (mode)
    {
    case CamPosInterp::Linear:
        return evalLinear ? evalLinear->Evaluate(time) : keys[(size_t)i].pos;
    case CamPosInterp::CatmullRom:
        return evalCatmull ? evalCatmull->Evaluate(time) : (evalLinear ? evalLinear->Evaluate(time) : keys[(size_t)i].pos);
    case CamPosInterp::Centripetal:
        return evalCentripetal ? evalCentripetal->Evaluate(time) : (evalLinear ? evalLinear->Evaluate(time) : keys[(size_t)i].pos);
    case CamPosInterp::BSpline:
        return evalBSpline ? evalBSpline->Evaluate(time) : (evalLinear ? evalLinear->Evaluate(time) : keys[(size_t)i].pos);
    case CamPosInterp::Hermite:
        return evalHermite ? evalHermite->Evaluate(time) : (evalLinear ? evalLinear->Evaluate(time) : keys[(size_t)i].pos);
    }

    return evalLinear ? evalLinear->Evaluate(time) : keys[(size_t)i].pos;
}

CamPosInterp CCamPosPerSegmentEvaluator::ResolveMode(_uint segIdx) const
{
    assert(seq);
    assert(keyframes);

    const auto& keys = *keyframes;

    if (keys.size() < 2) return seq->posInterp;
    if (segIdx >= (_uint)keys.size() - 1) return seq->posInterp;

    const CamKeyFrame& k = keys[(size_t)segIdx];
    if (k.useCustomInterp) return k.outPosInterp;

    return seq->posInterp;
}

void CCamPosPerSegmentEvaluator::Free()
{
    Safe_Release(evalLinear);
    Safe_Release(evalCatmull);
    Safe_Release(evalCentripetal);
    Safe_Release(evalBSpline);
    Safe_Release(evalHermite);

    __super::Free();
}