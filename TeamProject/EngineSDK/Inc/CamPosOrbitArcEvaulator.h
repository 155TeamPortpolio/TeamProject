#pragma once

#include "ICamEvaluator.h"
#include "OrbitArcData.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamPosOrbitArcEvaluator final : public ICamPosEvaluator
{
private:
    CCamPosOrbitArcEvaluator() {}
	virtual ~CCamPosOrbitArcEvaluator() DEFAULT;

public:
    bool     Build(const vector<CamKeyFrame>& keys) override;
    _vector3 Evaluate(_float time) const override;

public:
    void     SetOrbitDesc(const CamOrbitArcDesc* desc) { orbitDesc = desc; }

private:
    const vector<CamKeyFrame>* keyframes{};
    const CamOrbitArcDesc* orbitDesc{};

public:
    static CCamPosOrbitArcEvaluator* Create() { return new CCamPosOrbitArcEvaluator(); }
};

NS_END