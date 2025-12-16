#include "Engine_Defines.h"
#include "CamEvaluator.h"

bool CCamEvaluator::Build(const CamSequenceDesc& _seqDesc)
{
	seqDesc = &_seqDesc;

	cachedKeys = _seqDesc.keyframes;
	if (cachedKeys.size() < 2) return false;

	stable_sort(cachedKeys.begin(), cachedKeys.end(), [](const CamKeyFrame& a, const CamKeyFrame& b) { return a.time < b.time; });

	vector<CamKeyFrame> merged;
	merged.reserve(cachedKeys.size());

	constexpr float kEps = 1e-4f;

	for (size_t i = 0; i < cachedKeys.size(); ++i)
	{
		const CamKeyFrame& cur = cachedKeys[i];

		if (merged.empty())
		{
			merged.push_back(cur);
			continue;
		}

		CamKeyFrame& last = merged.back();

		if (fabsf(cur.time - last.time) <= kEps) last = cur;
		else merged.push_back(cur);
	}

	cachedKeys.swap(merged);

	if (cachedKeys.size() < 2) return false;

	duration = cachedKeys.back().time;
	if (duration <= 0.f) return false;

	if (!posEval || !rotEval || !fovEval) return false;

	if (!posEval->Build(cachedKeys)) return false;
	if (!rotEval->Build(cachedKeys)) return false;
	if (!fovEval->Build(cachedKeys)) return false;

	return true;
}

CamPose CCamEvaluator::Evaluate(float playTime) const
{
	const float t = MapTime(playTime);

	CamPose pose{};
	pose.pos = posEval->Evaluate(t);
	pose.rot = rotEval->Evaluate(t);
	pose.fov = fovEval->Evaluate(t);
	return pose;
}

void CCamEvaluator::SetPosEvaluator(ICamPosEvaluator* _posEval)
{
	Safe_Release(posEval);
	posEval = _posEval;
}

void CCamEvaluator::SetRotEvaluator(ICamRotEvaluator* _rotEval)
{
	Safe_Release(rotEval);
	rotEval = _rotEval;
}

void CCamEvaluator::SetFovEvaluator(ICamFovEvaluator* _fovEval)
{
	Safe_Release(fovEval);
	fovEval = _fovEval;
}

_float CCamEvaluator::MapTime(float playTime) const
{
	const float dur = duration;
	if (dur <= 0.f) return 0.f;

	switch (seqDesc->playbackMode)
	{
	case CamPlaybackMode::Once:
		return clamp(playTime, 0.f, dur);

	case CamPlaybackMode::Loop:
	{
		float t = fmodf(playTime, dur);
		if (t < 0.f)
			t += dur;
		return t;
	}

	case CamPlaybackMode::PingPong:
	{
		const float period = dur * 2.f;
		float x = fmodf(playTime, period);
		if (x < 0.f)
			x += period;
		return (x <= dur) ? x : (period - x);
	}
	}
	return playTime;
}

void CCamEvaluator::Free()
{
	__super::Free();
	Safe_Release(posEval);
	Safe_Release(rotEval);
	Safe_Release(fovEval);
}