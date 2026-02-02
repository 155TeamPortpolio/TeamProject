#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamPosPerSegmentEvaluator final : public ICamPosEvaluator
{
private:
    CCamPosPerSegmentEvaluator() {}
    virtual ~CCamPosPerSegmentEvaluator() DEFAULT;

public:
    void         SetSequence(const CamSequenceDesc* s) { seq = s; }         
    bool         Build(const vector<CamKeyFrame>& keys) override;
    _vector3     Evaluate(_float time) const override;

private:
    CamPosInterp ResolveMode(_uint segIdx) const;

private:
    const CamSequenceDesc*     seq{};
    const vector<CamKeyFrame>* keyframes{};

    ICamPosEvaluator*          evalLinear{};
    ICamPosEvaluator*          evalCatmull{};
    ICamPosEvaluator*          evalCentripetal{};
    ICamPosEvaluator*          evalBSpline{};
    ICamPosEvaluator*          evalHermite{};
    ICamPosEvaluator*          evalOrbitArc{};
    ICamPosEvaluator*          evalOrbitSpin{};

public:
    static CCamPosPerSegmentEvaluator* Create() { return new CCamPosPerSegmentEvaluator(); }
    virtual void Free() override;
};

NS_END