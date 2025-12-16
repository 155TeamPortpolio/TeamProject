#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamRotPerSegmentEvaluator final : public ICamRotEvaluator
{
private:
    CCamRotPerSegmentEvaluator() {}
    virtual ~CCamRotPerSegmentEvaluator() DEFAULT;

public:
    void         SetSequence(const CamSequenceDesc* s) { seq = s; }
    bool         Build(const vector<CamKeyFrame>& keys) override;
    Quaternion   Evaluate(_float time) const override;

private:
    CamRotInterp ResolveMode(_uint segIdx) const;

private:
    const CamSequenceDesc*     seq{};
    const vector<CamKeyFrame>* keyframes{};

    ICamRotEvaluator* evalSlerp{};
    ICamRotEvaluator* evalSquad{};

public:
    static CCamRotPerSegmentEvaluator* Create() { return new CCamRotPerSegmentEvaluator(); }
    virtual void Free() override;
};

NS_END

