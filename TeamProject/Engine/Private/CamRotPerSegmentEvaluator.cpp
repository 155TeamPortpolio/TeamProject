#include "Engine_Defines.h"
#include "CamRotPerSegmentEvaluator.h"
#include "CamRotSlerpEvaluator.h"
#include "CamRotSquadEvaluator.h"

namespace
{
    float WrapPi(float a)
    {
        while (a >  XM_PI)  a -= XM_2PI;
        while (a < -XM_PI) a += XM_2PI;
        return a;
    }

    float LerpAngle(float a, float b, float u)
    {
        float d = WrapPi(b - a);
        return a + d * u;
    }
}

bool CCamRotPerSegmentEvaluator::Build(const vector<CamKeyFrame>& keys)
{
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
    const auto& keys = *keyframes;

    if (keys.size() < 2) return seq->rotInterp;
    if (segIdx >= (_uint)keys.size() - 1) return seq->rotInterp;

    const CamKeyFrame& k = keys[(size_t)segIdx];
    if (k.useCustomInterp) return k.outRotInterp;

    return seq->rotInterp;
}

Quaternion CCamRotPerSegmentEvaluator::Evaluate(_float time) const
{
    const auto& keys = *keyframes;
    const size_t n = keys.size();

    if (n == 0)
    {
        lastRoll = 0.f;
        return Quaternion::Identity;
    }

    if (n == 1)
    {
        lastRoll = keys[0].roll;

        const bool lookAt = (seq && seq->boneAttach.enabled && seq->boneAttach.mode == CamBoneMode::LookAt);
        if (lookAt) return Quaternion::Identity;

        return evalSlerp ? evalSlerp->Evaluate(keys[0].time) : Quaternion::Identity;
    }

    float t = time;
    if (t <= keys[0].time) t = keys[0].time;
    if (t >= keys.back().time) t = keys.back().time;

    const CamKeySegment segment = CamUtil::FindKeySegment(keys, t);
    const _uint i = segment.segmentIdx;

    const CamRotInterp mode = ResolveMode(i);

    if (mode == CamRotInterp::Hold)
        lastRoll = keys[(size_t)i].roll;
    else
    {
        const CamKeyFrame& k0 = keys[(size_t)i];
        const CamKeyFrame& k1 = keys[(size_t)i + 1];

        float u = segment.normalizedTime;
        u = clamp(u, 0.f, 1.f);

        lastRoll = LerpAngle(k0.roll, k1.roll, u);
    }

    const bool lookAt = (seq && seq->boneAttach.enabled && seq->boneAttach.mode == CamBoneMode::LookAt);
    if (lookAt) return Quaternion::Identity;

    switch (mode)
    {
    case CamRotInterp::Slerp:
        return evalSlerp ? evalSlerp->Evaluate(t) : Quaternion::Identity;
    case CamRotInterp::Squad:
        return evalSquad ? evalSquad->Evaluate(t) : (evalSlerp ? evalSlerp->Evaluate(t) : Quaternion::Identity);
    case CamRotInterp::Hold:
        return evalSlerp ? evalSlerp->Evaluate(keys[(size_t)i].time) : Quaternion::Identity;
    }

    return evalSlerp ? evalSlerp->Evaluate(t) : Quaternion::Identity;
}

void CCamRotPerSegmentEvaluator::Free()
{
    Safe_Release(evalSlerp);
    Safe_Release(evalSquad);

    __super::Free();
}