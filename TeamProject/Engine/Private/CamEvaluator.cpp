#include "Engine_Defines.h"
#include "CamEvaluator.h"

bool CamEvaluator::Build(const CamSequenceDesc& _seqDesc)
{
	if (_seqDesc.keyframes.size() < 2)
		return false;

	seqDesc = &_seqDesc;
	duration = seqDesc->GetDuration();
	if (duration <= 0.f)
		return false;

	if (!posEval || !rotEval || !fovEval)
		return false;

	if (!posEval->Build(_seqDesc.keyframes)) return false;
	if (!rotEval->Build(_seqDesc.keyframes)) return false;
	if (!fovEval->Build(_seqDesc.keyframes)) return false;

	return true;
}

CamPose CamEvaluator::Evaluate(float playTime) const
{
	const float t = MapTime(playTime);

	CamPose pose{};
	pose.pos = posEval->Evaluate(t);
	pose.rot = rotEval->Evaluate(t);
	pose.fov = fovEval->Evaluate(t);
	return pose;
}

void CamEvaluator::SetPosEvaluator(ICamPosEvaluator* _posEval)
{
	Safe_Release(posEval);
	posEval = _posEval;
}

void CamEvaluator::SetRotEvaluator(ICamRotEvaluator* _rotEval)
{
	Safe_Release(rotEval);
	rotEval = _rotEval;
}

void CamEvaluator::SetFovEvaluator(ICamFovEvaluator* _fovEval)
{
	Safe_Release(fovEval);
	fovEval = _fovEval;
}

_float CamEvaluator::MapTime(float playTime) const
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

void CamEvaluator::Free()
{
	__super::Free();
	Safe_Release(posEval);
	Safe_Release(rotEval);
	Safe_Release(fovEval);
}