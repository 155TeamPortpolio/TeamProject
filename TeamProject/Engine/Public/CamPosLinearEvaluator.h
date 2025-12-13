#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamPosLinearEvaluator final : public ICamPosEvaluator
{
public:
	bool     Build(const vector<CamKeyFrame>& keys) override;
	_vector3 Evaluate(_float time) const override;

private:
	const vector<CamKeyFrame>* keyframes{};
};

NS_END