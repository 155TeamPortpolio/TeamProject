#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamFovSmoothEvaluator final : public ICamFovEvaluator
{
private:
	CCamFovSmoothEvaluator() {}
	virtual ~CCamFovSmoothEvaluator() DEFAULT;

public:
	bool   Build(const vector<CamKeyFrame>& keys) override;
	_float Evaluate(_float time) const override;

private:
	const vector<CamKeyFrame>* keyframes{};

public:
	static CCamFovSmoothEvaluator* Create() { return new CCamFovSmoothEvaluator(); }
};

NS_END