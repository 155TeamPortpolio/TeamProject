#include "Engine_Defines.h"
#include "CamRotPerSegmentEvaluator.h"
#include "CamRotSlerpEvaluator.h"
#include "CamRotSquadEvaluator.h"

bool CCamRotPerSegmentEvaluator::Build(const vector<CamKeyFrame>& keys)
{
    assert(seq);

    keyframes = &keys;

    Safe_Release(evalSlerp);
    Safe_Release(evalSquad);

    evalSlerp = CCamRotSlerpEvaluator::Create();
    evalSquad = CCamRotSquadEvaluator::Create();

    bool ok = true;
    ok &= evalSlerp->Build(keys);
    ok &= evalSquad->Build(keys);

    return ok;
}

CamRotInterp CCamRotPerSegmentEvaluator::ResolveMode(_uint segIdx) const
{
    assert(seq);
    assert(keyframes);

    const auto& keys = *keyframes;

    if (keys.size() < 2) return seq->rotInterp;
    if (segIdx >= (_uint)keys.size() - 1) return seq->rotInterp;

    const CamKeyFrame& k = keys[(size_t)segIdx];
    if (k.useCustomInterp) return k.outRotInterp;

    return seq->rotInterp;
}

Quaternion CCamRotPerSegmentEvaluator::Evaluate(_float time) const
{
    assert(seq);
    assert(keyframes);

    const auto& keys = *keyframes;
    const size_t n = keys.size();

    if (n == 0) return Quaternion::Identity;
    if (n == 1)
    {
        if (!evalSlerp) return Quaternion::Identity;
        return evalSlerp->Evaluate(keys[0].time);
    }

    float t = time;
    if (t <= keys[0].time) t = keys[0].time;
    if (t >= keys.back().time) t = keys.back().time;

    const CamKeySegment segment = CamUtil::FindKeySegment(keys, t);
    const _uint i = segment.segmentIdx;

    const CamRotInterp mode = ResolveMode(i);

    switch (mode)
    {
    case CamRotInterp::Slerp:
        return evalSlerp ? evalSlerp->Evaluate(t) : Quaternion::Identity;
    case CamRotInterp::Squad:
        return evalSquad ? evalSquad->Evaluate(t) : (evalSlerp ? evalSlerp->Evaluate(t) : Quaternion::Identity);
    }

    return evalSlerp ? evalSlerp->Evaluate(t) : Quaternion::Identity;
}


void CCamRotPerSegmentEvaluator::Free()
{
    Safe_Release(evalSlerp);
    Safe_Release(evalSquad);

    __super::Free();
}