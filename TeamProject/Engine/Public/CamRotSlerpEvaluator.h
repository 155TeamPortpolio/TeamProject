#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamRotSlerpEvaluator final : public ICamRotEvaluator
{
private:
	CCamRotSlerpEvaluator() {}
	virtual ~CCamRotSlerpEvaluator() DEFAULT;

public:
	bool       Build(const vector<CamKeyFrame>& keys) override;
	Quaternion Evaluate(_float time) const override;

private:
	Quaternion MakeRotFromLookRoll(_vector3 look, _float rollRad) const;

private:
	const vector<CamKeyFrame>* keyframes{};
	vector<Quaternion>         cachedRots{};

public:
	static CCamRotSlerpEvaluator* Create() { return new CCamRotSlerpEvaluator(); }
};

NS_END