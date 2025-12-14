#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamFovLinearEvaluator final : public ICamFovEvaluator
{
public:
	bool   Build(const vector<CamKeyFrame>& keys) override;
	_float Evaluate(float time) const override;

private:
	const vector<CamKeyFrame>* keyframes{};

public:
	static CamFovLinearEvaluator* Create() { return new CamFovLinearEvaluator(); }
	virtual void Free() override { __super::Free(); }
};

NS_END