#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)
// Centripetal Catmull-Rom: 점 사이 거리에 따라 t 간격을 비균일하게 잡음("멀면 더 큰 간격, 가까우면)
class ENGINE_DLL CamPosCentripetalEvaluator final : public ICamPosEvaluator
{
public:
	bool     Build(const vector<CamKeyFrame>& keys) override;
	_vector3 Evaluate(_float time) const override;

private:
	_vector3 GetPosClamped(int idx) const;

private:
	const vector<CamKeyFrame>* keyframes{};
	_float alpha = 0.5f;

public:
	static CamPosCentripetalEvaluator* Create() { return new CamPosCentripetalEvaluator(); }
	virtual void Free() override { __super::Free(); }
};

NS_END