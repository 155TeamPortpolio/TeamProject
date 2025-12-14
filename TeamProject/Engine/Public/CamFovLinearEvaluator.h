#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamFovLinearEvaluator final : public ICamFovEvaluator
{
private:
	CCamFovLinearEvaluator() {}
	virtual ~CCamFovLinearEvaluator() DEFAULT;

public:
	bool   Build(const vector<CamKeyFrame>& keys) override;
	_float Evaluate(float time) const override;

private:
	const vector<CamKeyFrame>* keyframes{};

public:
	static CCamFovLinearEvaluator* Create() { return new CCamFovLinearEvaluator(); }
	virtual void Free() override { __super::Free(); }
};

NS_END