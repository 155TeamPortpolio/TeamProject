#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamRotSlerpEvaluator final : public ICamRotEvaluator
{
private:
	CamRotSlerpEvaluator() = default;
	virtual ~CamRotSlerpEvaluator() = default;

public:
	bool       Build(const vector<CamKeyFrame>& keys) override;
	Quaternion Evaluate(_float time) const override;

private:
	Quaternion MakeRotFromLookRoll(_vector3 look, _float rollRad) const;

private:
	const vector<CamKeyFrame>* keyframes{};
	vector<Quaternion>         cachedRots{};

public:
	static CamRotSlerpEvaluator* Create() { return new CamRotSlerpEvaluator(); }
	virtual void Free() override { __super::Free(); }
};

NS_END