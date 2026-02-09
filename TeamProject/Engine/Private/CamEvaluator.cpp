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

    Quaternion MakeLookRotationStable(const Vector3& forward, const Vector3& upHint)
    {
        Vector3 f = forward;
        Vector3 u = upHint;

        if (f.LengthSquared() <= 1e-12f) f = Vector3(0.f, 0.f, 1.f);
        else f.Normalize();

        if (u.LengthSquared() <= 1e-12f) u = Vector3(0.f, 1.f, 0.f);
        else u.Normalize();

        Vector3 fb = -f;

        const float parallel = fabsf(fb.Dot(u));
        if (parallel > 0.999f)
        {
            Vector3 alt = Vector3(0.f, 0.f, 1.f);
            if (fabsf(fb.Dot(alt)) > 0.999f) alt = Vector3(1.f, 0.f, 0.f);
            u = alt;
        }

        Vector3 right = u.Cross(fb);
        if (right.LengthSquared() <= 1e-12f) right = Vector3(1.f, 0.f, 0.f);
        else right.Normalize();

        Vector3 up = fb.Cross(right);
        if (up.LengthSquared() <= 1e-12f) up = Vector3(0.f, 1.f, 0.f);
        else up.Normalize();

        Matrix basis;
        basis._11 = right.x; basis._12 = right.y; basis._13 = right.z; basis._14 = 0.f;
        basis._21 = up.x;    basis._22 = up.y;    basis._23 = up.z;    basis._24 = 0.f;
        basis._31 = fb.x;    basis._32 = fb.y;    basis._33 = fb.z;    basis._34 = 0.f;
        basis._41 = 0.f;     basis._42 = 0.f;     basis._43 = 0.f;     basis._44 = 1.f;

        Quaternion q = Quaternion::CreateFromRotationMatrix(basis);
        q.Normalize();
        return q;
    }
}

bool CCamEvaluator::Build(const CamSeqDesc& _seqDesc)
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
    pose.roll = rotEval->GetLastRoll();
    pose.fov = fovEval->Evaluate(time);

    if (!seqDesc) return pose;
    if (cachedKeys.size() < 2) return pose;

    float t = time;
    if (t <= cachedKeys.front().time) t = cachedKeys.front().time;
    if (t >= cachedKeys.back().time)  t = cachedKeys.back().time;

    const CamKeySegment seg = CamUtil::FindKeySegment(cachedKeys, t);
    const _uint i = seg.segmentIdx;

    auto ResolvePosMode = [&](uint32_t segIdx)
        {
            CamPosInterp mode = seqDesc->posInterp;
            if (segIdx < cachedKeys.size())
            {
                const CamKeyFrame& k0 = cachedKeys[(size_t)segIdx];
                if (k0.useCustomInterp) mode = k0.outPosInterp;
            }
            return mode;
        };

    const CamPosInterp posMode = ResolvePosMode(i);

    if (posMode != CamPosInterp::OrbitSpin) return pose;
    if (!seqDesc->orbitSpin.enabled) return pose;

    _uint startSeg = i;
    while (startSeg > 0)
    {
        if (ResolvePosMode(startSeg - 1) != CamPosInterp::OrbitSpin) break;
        --startSeg;
    }

    _uint endSeg = i;
    const _uint lastSeg = (_uint)cachedKeys.size() - 2;
    while (endSeg < lastSeg)
    {
        if (ResolvePosMode(endSeg + 1) != CamPosInterp::OrbitSpin) break;
        ++endSeg;
    }

    const float tStart = cachedKeys[(size_t)startSeg].time;
    const float tEnd = cachedKeys[(size_t)endSeg + 1].time;

    Vector3 center = Vector3::Zero;
    if (seqDesc->orbitSpin.centerMode == CamSpinCenterMode::Custom)
        center = Vector3(seqDesc->orbitSpin.center.x, seqDesc->orbitSpin.center.y, seqDesc->orbitSpin.center.z);

    Vector3 up = Vector3(0.f, 1.f, 0.f);
    if (!seqDesc->orbitSpin.keepHeight)
    {
        up = Vector3(seqDesc->orbitSpin.axis.x, seqDesc->orbitSpin.axis.y, seqDesc->orbitSpin.axis.z);
        if (up.LengthSquared() <= 1e-12f) up = Vector3(0.f, 1.f, 0.f);
        up.Normalize();
    }

    Vector3 p(pose.pos.x, pose.pos.y, pose.pos.z);
    Vector3 look = center - p;

    if (look.LengthSquared() <= 1e-8f) return pose;

    look.Normalize();

    Quaternion qLook = MakeLookRotation(_vector3(look.x, look.y, look.z), _vector3(up.x, up.y, up.z));
    qLook.Normalize();

    const float blendSec = 0.20f;

    const float inU = (t - tStart) / blendSec;
    const float outU = (tEnd - t) / blendSec;

    if (t <= tStart + blendSec)
    {
        Quaternion qInBase = rotEval->Evaluate(tStart);
        qInBase.Normalize();

        const float dot = qInBase.x * qLook.x + qInBase.y * qLook.y + qInBase.z * qLook.z + qInBase.w * qLook.w;
        if (dot < 0.f) qLook = Quaternion(-qLook.x, -qLook.y, -qLook.z, -qLook.w);

        float w = clamp(inU, 0.f, 1.f);
        w = Math::ApplyEase(EaseType::InOutSine, w);

        pose.rot = Quaternion::Slerp(qInBase, qLook, w);
        pose.rot.Normalize();
        return pose;
    }

    if (t >= tEnd - blendSec)
    {
        Quaternion qOutBase = rotEval->Evaluate(tEnd);
        qOutBase.Normalize();

        const float dot = qOutBase.x * qLook.x + qOutBase.y * qLook.y + qOutBase.z * qLook.z + qOutBase.w * qLook.w;
        if (dot < 0.f) qLook = Quaternion(-qLook.x, -qLook.y, -qLook.z, -qLook.w);

        float w = clamp(outU, 0.f, 1.f);
        w = Math::ApplyEase(EaseType::InOutSine, w);

        pose.rot = Quaternion::Slerp(qOutBase, qLook, w);
        pose.rot.Normalize();
        return pose;
    }

    pose.rot = qLook;
    pose.rot.Normalize();
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