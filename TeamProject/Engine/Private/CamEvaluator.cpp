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

CamPose CCamEvaluator::Evaluate(float time) const
{
	CamPose pose{};
	pose.pos = posEval->Evaluate(time);
	pose.rot = rotEval->Evaluate(time);
	pose.fov = fovEval->Evaluate(time);
	return pose;
}

_float CCamEvaluator::RemapTimeBySegmentEasing(float t) const
{
	if (cachedKeys.size() < 2)
		return t;

	if (t <= cachedKeys.front().time)
		return cachedKeys.front().time;

	if (t >= cachedKeys.back().time)
		return cachedKeys.back().time;

	_uint seg = 0;
	for (; seg + 1 < cachedKeys.size(); ++seg)
	{
		const float nextTime = cachedKeys[seg + 1].time;
		if (t < nextTime)
			break;
	}

	const CamKeyFrame& k0 = cachedKeys[seg];
	const CamKeyFrame& k1 = cachedKeys[seg + 1];

	EaseType ease = seqDesc ? seqDesc->segmentEase : EaseType::None;
	if (k0.useCustomEase)
		ease = k0.outEase;

	if (ease == EaseType::None)
		return t;

	const float t0 = k0.time;
	const float t1 = k1.time;

	float u = (t - t0) / (t1 - t0);
	u = clamp(u, 0.f, 1.f);

	u = Math::ApplyEase(ease, u);
	u = clamp(u, 0.f, 1.f);

	return Math::Lerp(t0, t1, u);
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

void CCamEvaluator::Free()
{
	__super::Free();
	Safe_Release(posEval);
	Safe_Release(rotEval);
	Safe_Release(fovEval);
}