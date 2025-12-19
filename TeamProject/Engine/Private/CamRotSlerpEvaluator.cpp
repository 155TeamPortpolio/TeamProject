#include "Engine_Defines.h"
#include "CamRotSlerpEvaluator.h"

bool CCamRotSlerpEvaluator::Build(const vector<CamKeyFrame>& keys)
{
	if (keys.empty())
		return false;

	keyframes = &keys;

	cachedRots.clear();
	cachedRots.resize(keys.size());

	for (size_t keyIdx = 0; keyIdx < keys.size(); ++keyIdx)
	{
		const CamKeyFrame& key = keys[keyIdx];

		const float lookLengthSq = key.look.LengthSquared();
		if (lookLengthSq < 1e-8f)
			return false;

		cachedRots[keyIdx] = MakeRotFromLookRoll(key.look, key.roll);
	}
	for (size_t keyIdx = 1; keyIdx < cachedRots.size(); ++keyIdx)
	{
		const float dotValue = cachedRots[keyIdx - 1].Dot(cachedRots[keyIdx]);
		if (dotValue < 0.f)
			cachedRots[keyIdx] = -cachedRots[keyIdx];
	}
	return true;
}

Quaternion CCamRotSlerpEvaluator::Evaluate(_float time) const
{
	if (keyframes->size() == 1)
		return cachedRots[0];

	const CamKeySegment segment = CamUtil::FindKeySegment(*keyframes, time);

	const _uint segmentIdx = segment.segmentIdx;
	const float u          = segment.normalizedTime;

	const Quaternion& startRot = cachedRots[segmentIdx];
	const Quaternion& endRot   = cachedRots[segmentIdx + 1];

	return Quaternion::Slerp(startRot, endRot, u);
}

Quaternion CCamRotSlerpEvaluator::MakeRotFromLookRoll(_vector3 look, _float rollRad) const
{
	if (look.LengthSquared() <= 1e-8f)
		look = _vector3(0.f, 0.f, 1.f);
	else
		look.Normalize();

	const _vector3 forward = look;

	_vector3 referenceUp(0.f, 1.f, 0.f);
	const float parallel = fabsf(forward.Dot(referenceUp));
	if (parallel > 0.999f)
		referenceUp = _vector3(0.f, 0.f, 1.f);

	_vector3 right = referenceUp.Cross(forward);
	right.Normalize();

	_vector3 up = forward.Cross(right);
	up.Normalize();

	Matrix basis;
	basis._11 = right.x;   basis._12 = right.y;   basis._13 = right.z;   basis._14 = 0.f;
	basis._21 = up.x;      basis._22 = up.y;      basis._23 = up.z;      basis._24 = 0.f;
	basis._31 = forward.x; basis._32 = forward.y; basis._33 = forward.z; basis._34 = 0.f;
	basis._41 = 0.f;       basis._42 = 0.f;       basis._43 = 0.f;       basis._44 = 1.f;

	Quaternion baseRot = Quaternion::CreateFromRotationMatrix(basis);

	Quaternion rollRot = Quaternion::CreateFromAxisAngle(forward, rollRad);
	Quaternion finalRot = rollRot * baseRot;
	finalRot.Normalize();
	return finalRot;
}