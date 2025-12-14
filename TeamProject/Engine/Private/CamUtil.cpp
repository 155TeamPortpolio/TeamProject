#include "Engine_Defines.h"
#include "CamUtil.h"
#include "Camera_Struct.h"

CamKeySegment CamUtil::FindKeySegment(const vector<CamKeyFrame>& keyframes, float time)
{
	assert(keyframes.size() >= 2);

	const float firstTime = keyframes.front().time;
	const float lastTime  = keyframes.back().time;

	if (time <= firstTime)
		return { 0, 0.f };

	const _uint lastIdx = keyframes.size() - 1;
	if (time >= lastTime)
		return { lastIdx - 1, 1.f };

	_uint segmentIdx = 0;

	for (; segmentIdx + 1 < keyframes.size(); ++segmentIdx)
	{
		const float nextTime = keyframes[segmentIdx + 1].time;
		if (time < nextTime)
			break;
	}

	const float segmentStartTime = keyframes[segmentIdx].time;
	const float segmentEndTime   = keyframes[segmentIdx + 1].time;

	const float segmentDuration = segmentEndTime - segmentStartTime;
	assert(segmentDuration > 0.f);

	float normalizedTime = (time - segmentStartTime) / segmentDuration;
	normalizedTime = clamp(normalizedTime, 0.f, 1.f);

	return { segmentIdx, normalizedTime };
}
