#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamEvaluator final : public CBase
{
private:
    CCamEvaluator() {}
    virtual ~CCamEvaluator() DEFAULT;

public:
    bool    Build(const CamSequenceDesc& seqDesc);
    CamPose Evaluate(float time) const;

    _float RemapTimeBySegmentEasing(float t) const;

    void SetPosEvaluator(ICamPosEvaluator* _posEval);
    void SetRotEvaluator(ICamRotEvaluator* _rotEval);
    void SetFovEvaluator(ICamFovEvaluator* _fovEval);

    _float GetDuration() const { return duration; }

private:
    const CamSequenceDesc* seqDesc{};
    vector<CamKeyFrame>    cachedKeys;
    _float                 duration{};

    ICamPosEvaluator* posEval{};
    ICamRotEvaluator* rotEval{};
    ICamFovEvaluator* fovEval{};

public:
    static CCamEvaluator* Create() { return new CCamEvaluator(); }
    virtual void Free() override;
};

NS_END