#include "Engine_Defines.h"
#include "CamPosLinearEvaluator.h"

bool CCamPosLinearEvaluator::Build(const vector<CamKeyFrame>& keys)
{
	if (keys.empty())
		return false;

	keyframes = &keys;
	return true;
}

_vector3 CCamPosLinearEvaluator::Evaluate(_float time) const
{
    if (keyframes->size() == 1)
        return (*keyframes)[0].pos;

    const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

    const _uint segmentIdx = segment.segmentIdx;
    const float u = segment.normalizedTime;

    const _vector3& startPos = (*keyframes)[segmentIdx].pos;
    const _vector3& endPos = (*keyframes)[segmentIdx + 1].pos;

    return startPos + (endPos - startPos) * u;
}