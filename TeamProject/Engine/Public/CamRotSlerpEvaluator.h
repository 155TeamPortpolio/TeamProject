#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class CamRotSlerpEvaluator final : public ICamRotEvaluator
{
public:
	bool Build(const vector<CamKeyFrame>& keys) override;
	Quaternion Evaluate(_float time) const override;

private:
	Quaternion MakeRotFromLookRoll(_vector3 look, _float rollRad) const;

private:
	const vector<CamKeyFrame>* keyframes{};
	vector<Quaternion>         cachedRots{};
};

NS_END