#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamPosLinearEvaluator final : public ICamPosEvaluator
{
private:
	CCamPosLinearEvaluator() {}
	virtual ~CCamPosLinearEvaluator() DEFAULT;

public:
	bool     Build(const vector<CamKeyFrame>& keys) override;
	_vector3 Evaluate(_float time) const override;

private:
	const vector<CamKeyFrame>* keyframes{};

public:
	static CCamPosLinearEvaluator* Create() { return new CCamPosLinearEvaluator(); }
	virtual void Free() override { __super::Free(); }
};

NS_END