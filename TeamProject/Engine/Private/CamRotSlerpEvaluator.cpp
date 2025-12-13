#include "Engine_Defines.h"
#include "CamRotSlerpEvaluator.h"

bool CamRotSlerpEvaluator::Build(const vector<CamKeyFrame>& keys)
{
	if (keys.size() < 2)
		return false;

	keyframes = &keys;

	cachedRots.clear();
	cachedRots.resize(keys.size());

	// 1. look + roll -> quaterion 변환 캐시
	for (size_t keyIdx = 0; keyIdx < keys.size(); ++keyIdx)
	{
		const CamKeyFrame& key = keys[keyIdx];
		
		const float lookLengthSq = key.look.LengthSquared();
		if (lookLengthSq < 1e-8f)
			return false;

		cachedRots[keyIdx] = MakeRotFromLookRoll(key.look, key.roll);
	}
	// 2. hemisphere 정리 (연속성 유지: dot < 0 이면 부호 반전)
	for (size_t keyIdx = 1; keyIdx < cachedRots.size(); ++keyIdx)
	{
		const float dotValue = cachedRots[keyIdx - 1].Dot(cachedRots[keyIdx]);
		if (dotValue < 0.f)
			cachedRots[keyIdx] = -cachedRots[keyIdx];
	}
	return true;
}

Quaternion CamRotSlerpEvaluator::Evaluate(_float time) const
{
	assert(keyframes);
	assert(keyframes->size() >= 2);
	assert(cachedRots.size() == keyframes->size());

	const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

	const _uint segmentIdx = segment.segmentIdx;
	const float u          = segment.normalizedTime;

	const Quaternion& startRot = cachedRots[segmentIdx];
	const Quaternion& endRot = cachedRots[segmentIdx + 1];

	return Quaternion::Slerp(startRot, endRot, u);
}

Quaternion CamRotSlerpEvaluator::MakeRotFromLookRoll(_vector3 look, _float rollRad) const
{
	look.Normalize();
	const _vector3 forward = look;

	_vector3 referenceUp{ 0.f, 1.f, 0.f };
	const float parallel = fabsf(forward.Dot(referenceUp));
	if (parallel > 0.999f)
		referenceUp = { 0.f, 0.f, 1.f };

	_vector3 right = referenceUp.Cross(forward);
	right.Normalize();

	_vector3 up = forward.Cross(right);
	up.Normalize();

	Matrix     basis    = Matrix::CreateWorld(Vector3::Zero, forward, up);
	Quaternion baseRot  = Quaternion::CreateFromRotationMatrix(basis);
	Quaternion rollRot  = Quaternion::CreateFromAxisAngle(forward, rollRad);
	Quaternion finalRot = baseRot * rollRot;
	finalRot.Normalize();
	return finalRot;
}