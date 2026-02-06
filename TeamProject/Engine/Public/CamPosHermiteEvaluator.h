#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamPosHermiteEvaluator final : public ICamPosEvaluator
{
private:
    CCamPosHermiteEvaluator() {}
    virtual ~CCamPosHermiteEvaluator() DEFAULT;

public:
    bool     Build(const vector<CamKeyFrame>& keys) override;
    _vector3 Evaluate(_float time) const override;

    void     SetTension(_float t) { tension = clamp(t, 0.f, 1.f); }

private:
    _vector3 GetPosClamped(int idx) const;
    _vector3 Hermite(const _vector3& p1, const _vector3& p2, const _vector3& m1, const _vector3& m2, float u) const;

private:
    const vector<CamKeyFrame>* keyframes{};
    _float tension = 0.f;

public:
    static CCamPosHermiteEvaluator* Create() { return new CCamPosHermiteEvaluator(); }
};

NS_END