#include "Engine_Defines.h"
#include "CamEvaluator.h"

namespace
{
    Quaternion MakeLookRotationWorldUp(const Vector3& forward, const Quaternion& qRef)
    {
        Vector3 f = forward;
        if (f.LengthSquared() <= 1e-12f) f = Vector3(0.f, 0.f, 1.f);
        else f.Normalize();

        Vector3 referenceUp(0.f, 1.f, 0.f);
        if (fabsf(f.Dot(referenceUp)) > 0.999f) referenceUp = Vector3(0.f, 0.f, 1.f);

        Vector3 right = referenceUp.Cross(f);
        if (right.LengthSquared() <= 1e-12f) right = Vector3(1.f, 0.f, 0.f);
        else right.Normalize();

        Vector3 up = f.Cross(right);
        if (up.LengthSquared() <= 1e-12f) up = Vector3(0.f, 1.f, 0.f);
        else up.Normalize();

        Matrix basis;
        basis._11 = right.x; basis._12 = right.y; basis._13 = right.z; basis._14 = 0.f;
        basis._21 = up.x;    basis._22 = up.y;    basis._23 = up.z;    basis._24 = 0.f;
        basis._31 = f.x;     basis._32 = f.y;     basis._33 = f.z;     basis._34 = 0.f;
        basis._41 = 0.f;     basis._42 = 0.f;     basis._43 = 0.f;     basis._44 = 1.f;

        Quaternion q = Quaternion::CreateFromRotationMatrix(basis);
        q.Normalize();

        if (qRef.Dot(q) < 0.f) q = -q;
        q.Normalize();
        return q;
    }

    Vector3 LerpVec3(const Vector3& a, const Vector3& b, float t)
    {
        return a + (b - a) * t;
    }

    float SafeBlendSec(float requested, float segDur)
    {
        if (segDur <= 1e-6f) return 0.f;
        float s = requested;
        s = min(s, segDur * 0.25f);
        if (s < 0.f) s = 0.f;
        return s;
    }
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

    const float segDur = tEnd - tStart;

    Vector3 pCur(pose.pos.x, pose.pos.y, pose.pos.z);
    Quaternion qRef = pose.rot;
    qRef.Normalize();

    Vector3 pStart(posEval->Evaluate(tStart).x, posEval->Evaluate(tStart).y, posEval->Evaluate(tStart).z);
    Vector3 pEnd(posEval->Evaluate(tEnd).x, posEval->Evaluate(tEnd).y, posEval->Evaluate(tEnd).z);

    Quaternion rStart = rotEval->Evaluate(tStart); rStart.Normalize();
    Quaternion rEnd = rotEval->Evaluate(tEnd);   rEnd.Normalize();

    Matrix mStart = Matrix::CreateFromQuaternion(rStart);
    Matrix mEnd = Matrix::CreateFromQuaternion(rEnd);

    Vector3 lookDirStart = Vector3::Transform(Vector3(0.f, 0.f, 1.f), mStart);
    Vector3 lookDirEnd = Vector3::Transform(Vector3(0.f, 0.f, 1.f), mEnd);

    if (lookDirStart.LengthSquared() <= 1e-12f) lookDirStart = Vector3(0.f, 0.f, 1.f);
    else lookDirStart.Normalize();

    if (lookDirEnd.LengthSquared() <= 1e-12f) lookDirEnd = Vector3(0.f, 0.f, 1.f);
    else lookDirEnd.Normalize();

    float distStart = (center - pStart).Length();
    float distEnd = (center - pEnd).Length();

    if (distStart < 0.25f) distStart = 0.25f;
    if (distEnd < 0.25f) distEnd = 0.25f;

    Vector3 pivotStart = pStart + lookDirStart * distStart;
    Vector3 pivotEnd = pEnd + lookDirEnd * distEnd;

    float blendSec = SafeBlendSec(0.20f, segDur);

    Vector3 pivot = center;

    if (blendSec > 0.f && t < tStart + blendSec)
    {
        float u = (t - tStart) / blendSec;
        u = clamp(u, 0.f, 1.f);
        u = Math::ApplyEase(EaseType::InOutSine, u);
        pivot = LerpVec3(pivotStart, center, u);
    }
    else if (blendSec > 0.f && t > tEnd - blendSec)
    {
        float u = (t - (tEnd - blendSec)) / blendSec;
        u = clamp(u, 0.f, 1.f);
        u = Math::ApplyEase(EaseType::InOutSine, u);
        pivot = LerpVec3(center, pivotEnd, u);
    }

    Vector3 look = pivot - pCur;
    if (look.LengthSquared() <= 1e-8f) return pose;
    look.Normalize();

    pose.rot = MakeLookRotationWorldUp(look, qRef);
    pose.rot.Normalize();
    return pose;
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