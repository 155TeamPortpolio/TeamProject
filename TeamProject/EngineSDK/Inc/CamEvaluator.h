#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamEvaluator final : public CBase
{
public:
    bool    Build(const CamSequenceDesc& seqDesc);
    CamPose Evaluate(float playTime) const;

    void SetPosEvaluator(ICamPosEvaluator* _posEval);
    void SetRotEvaluator(ICamRotEvaluator* _rotEval);
    void SetFovEvaluator(ICamFovEvaluator* _fovEval);

    _float GetDuration() const { return duration; }

private:
    _float MapTime(float playTime) const;

private:
    const CamSequenceDesc* seqDesc{};
    _float duration{};

    ICamPosEvaluator* posEval{};
    ICamRotEvaluator* rotEval{};
    ICamFovEvaluator* fovEval{};

public:
    static CamEvaluator* Create() { return new CamEvaluator(); }
    virtual void Free() override;
};

NS_END