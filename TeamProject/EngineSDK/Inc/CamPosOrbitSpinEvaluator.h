#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamPosOrbitSpinEvaluator final : public ICamPosEvaluator
{
private:
    CCamPosOrbitSpinEvaluator() {}
    virtual ~CCamPosOrbitSpinEvaluator() DEFAULT;

public:
    void     SetOrbitDesc(const CamOrbitSpinDesc* d) { desc = d; }
    bool     Build(const vector<CamKeyFrame>& keys) override;
    _vector3 Evaluate(_float time) const override;

private:
    struct SegmentCache
    {
        Vector3 center = Vector3::Zero;
        Vector3 axisN = Vector3(0.f, 1.f, 0.f);

        Vector3 aPar = Vector3::Zero;
        Vector3 bPar = Vector3::Zero;

        Vector3 aDir = Vector3(1.f, 0.f, 0.f);

        float   aLen = 0.f;
        float   bLen = 0.f;

        float   totalAngle = 0.f;

        float   baseY = 0.f;
        _bool   keepHeight = true;
    };

private:
    Vector3  ResolveCenter() const;
    Vector3  ResolveAxisN(_bool keepHeight) const;
    void     BuildSegmentCache(const CamKeyFrame& k0, const CamKeyFrame& k1, SegmentCache& out) const;

private:
    const vector<CamKeyFrame>* keyframes{};
    const CamOrbitSpinDesc* desc{};
    vector<SegmentCache>    cache{};

public:
    static CCamPosOrbitSpinEvaluator* Create() { return new CCamPosOrbitSpinEvaluator(); }
};

NS_END
