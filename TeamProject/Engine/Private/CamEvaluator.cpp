#include "Engine_Defines.h"
#include "CamEvaluator.h"

namespace
{
	Quaternion MakeLookRotation(const _vector3& forward, const _vector3& up)
	{
		_vector3 f = forward;
		_vector3 u = up;

		f.Normalize();
		u.Normalize();

		Matrix m = Matrix::CreateWorld(_vector3::Zero, -f, u);

		Quaternion q = Quaternion::CreateFromRotationMatrix(m);
		q.Normalize();
		return q;
	}
}

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
	pose.pos  = posEval->Evaluate(time);
	pose.rot  = rotEval->Evaluate(time);
	pose.roll = rotEval->GetLastRoll();
	pose.fov  = fovEval->Evaluate(time);

	if (!seqDesc) return pose;
	if (cachedKeys.size() < 2) return pose;

	float t = time;
	if (t <= cachedKeys.front().time) t = cachedKeys.front().time;
	if (t >= cachedKeys.back().time)  t = cachedKeys.back().time;

	const CamKeySegment seg = CamUtil::FindKeySegment(cachedKeys, t);
	const _uint i = seg.segmentIdx;

	CamPosInterp posMode = seqDesc->posInterp;
	if (i < (_uint)cachedKeys.size())
	{
		const CamKeyFrame& k0 = cachedKeys[(size_t)i];
		if (k0.useCustomInterp) posMode = k0.outPosInterp;
	}

	_bool orbit = false;
	_vector3 center{};

	//if (posMode == CamPosInterp::OrbitArc && seqDesc->orbitArc.enabled)
	//{
	//	orbit = true;
	//	center = seqDesc->orbitArc.center;
	//}
	if (posMode == CamPosInterp::OrbitSpin && seqDesc->orbitSpin.enabled)
	{
		orbit = true;
		center = seqDesc->orbitSpin.center;
	}

	if (orbit)
	{
		Vector3 p(pose.pos.x, pose.pos.y, pose.pos.z);
		Vector3 c(center.x, center.y, center.z);

		Vector3 look = c - p;

		if (look.LengthSquared() > 1e-8f)
		{
			look.Normalize();

			pose.rot = MakeLookRotation(_vector3(look.x, look.y, look.z), _vector3(0.f, 1.f, 0.f));
			pose.rot.Normalize();
		}
	}

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