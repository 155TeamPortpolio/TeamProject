#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamPosCatmullRomEvaluator final : public ICamPosEvaluator
{
public:
	bool     Build(const vector<CamKeyFrame>& keys) override;
	_vector3 Evaluate(_float time) const override;

private:
	_vector3 GetPosClamped(int idx) const;
	_vector3 CatmullRom(const _vector3& p0, const _vector3& p1, const _vector3& p2, const _vector3& p3, float u) const;

private:
	const vector<CamKeyFrame>* keyframes{};

public:
	static CamPosCatmullRomEvaluator* Create() { return new CamPosCatmullRomEvaluator(); }
	virtual void Free() override { __super::Free(); }
};

NS_END