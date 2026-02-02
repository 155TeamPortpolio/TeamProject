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
    static _vector3 NormalizeSafe(const _vector3& v, float eps = 1e-8f);
    static _vector3 ProjectToPlane(const _vector3& v, const _vector3& unitAxis);
    static _vector3 RotateAroundAxis(const _vector3& v, const _vector3& unitAxis, float angleRad);
    static float    SignedAngleAroundAxis(const _vector3& fromUnit, const _vector3& toUnit, const _vector3& unitAxis);

private:
    const vector<CamKeyFrame>* keyframes{};
    const CamOrbitArcDesc* orbitDesc{};

public:
    static CCamPosOrbitArcEvaluator* Create() { return new CCamPosOrbitArcEvaluator(); }
};

NS_END