#include "Engine_Defines.h"
#include "CamFovLinearEvaluator.h"

bool CamFovLinearEvaluator::Build(const vector<CamKeyFrame>& keys)
{
	if (keys.size() < 2)
		return false;

	keyframes = &keys;
	return true;
}

_float CamFovLinearEvaluator::Evaluate(float time) const
{
	assert(keyframes);
	assert(keyframes->size() >= 2);

	const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

	const _uint segmentIdx = segment.segmentIdx;
	const float u = segment.normalizedTime;

	const float startFov = (*keyframes)[segmentIdx].fov;
	const float endFov   = (*keyframes)[segmentIdx + 1].fov;

	return startFov + (endFov - startFov) * u;
}
