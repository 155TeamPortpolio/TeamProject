#include "Engine_Defines.h"
#include "CamPosLinearEvaluator.h"

bool CamPosLinearEvaluator::Build(const vector<CamKeyFrame>& keys)
{
	if (keys.size() < 2)
		return false;

	keyframes = &keys;
	return true;
}

_vector3 CamPosLinearEvaluator::Evaluate(_float time) const
{
	assert(keyframes);
	assert(keyframes->size() >= 2);

	const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

	const _uint segmentIdx = segment.segmentIdx;
	const float u = segment.normalizedTime;

	const _vector3& startPos = (*keyframes)[segmentIdx].pos;
	const _vector3& endPos   = (*keyframes)[segmentIdx + 1].pos;

	return startPos + (endPos - startPos) * u;
}