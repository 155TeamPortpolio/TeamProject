#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamFovPerSegmentEvaluator final : public ICamFovEvaluator
{
private:
    CCamFovPerSegmentEvaluator() {}
    virtual ~CCamFovPerSegmentEvaluator() DEFAULT;

public:
    void         SetSequence(const CamSequenceDesc* s) { seq = s; }
                 
    bool         Build(const vector<CamKeyFrame>& keys) override;
    _float       Evaluate(_float time) const override;

private:
    CamFovInterp ResolveMode(_uint segIdx) const;

private:
    const CamSequenceDesc*     seq{};
    const vector<CamKeyFrame>* keyframes{};

    ICamFovEvaluator*          evalLinear{};
    ICamFovEvaluator*          evalSmooth{};

public:
    static CCamFovPerSegmentEvaluator* Create() { return new CCamFovPerSegmentEvaluator(); }
    virtual void Free() override;
};


NS_END