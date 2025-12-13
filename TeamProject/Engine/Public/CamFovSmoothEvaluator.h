#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamFovSmoothEvaluator final : public ICamFovEvaluator
{
private:
	CamFovSmoothEvaluator() = default;
	virtual ~CamFovSmoothEvaluator() = default;

public:
	bool   Build(const vector<CamKeyFrame>& keys) override;
	_float Evaluate(_float time) const override;

private:
	const vector<CamKeyFrame>* keyframes{};

public:
	static CamFovSmoothEvaluator* Create() { return new CamFovSmoothEvaluator(); }
	virtual void Free() override { __super::Free(); }
};

NS_END