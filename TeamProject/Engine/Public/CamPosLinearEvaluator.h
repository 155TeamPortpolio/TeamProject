#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamPosLinearEvaluator final : public ICamPosEvaluator
{
private:
	CamPosLinearEvaluator() = default;
	virtual ~CamPosLinearEvaluator() = default;

public:
	bool     Build(const vector<CamKeyFrame>& keys) override;
	_vector3 Evaluate(_float time) const override;

private:
	const vector<CamKeyFrame>* keyframes{};

public:
	static CamPosLinearEvaluator* Create() { return new CamPosLinearEvaluator(); }
	virtual void Free() override { __super::Free(); }
};

NS_END