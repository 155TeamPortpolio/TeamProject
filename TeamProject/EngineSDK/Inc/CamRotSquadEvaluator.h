#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamRotSquadEvaluator final : public ICamRotEvaluator
{
private:
    CCamRotSquadEvaluator() {}
    virtual ~CCamRotSquadEvaluator() DEFAULT;

public:
    bool              Build(const vector<CamKeyFrame>& keys) override;
    Quaternion        Evaluate(_float time) const override;

private:
    Quaternion        MakeRotFromLookRoll(_vector3 look, _float rollRad) const;
                      
    Quaternion        GetRotClamped(int idx) const;
    Quaternion        GetTanClamped(int idx) const;
                      
    Quaternion        ComputeTangent(int idx) const;

    static Quaternion QLogUnit(const Quaternion& q);
    static Quaternion QExpPure(const Quaternion& q);
    static Quaternion QMul(const Quaternion& a, const Quaternion& b);
    static Quaternion QConjugate(const Quaternion& q);
    static Quaternion QNormalizeSafe(const Quaternion& q);

private:
    const vector<CamKeyFrame>* keyframes{};
    vector<Quaternion>         cachedRots{};
    vector<Quaternion>         cachedTans{};

public:
    static CCamRotSquadEvaluator* Create() { return new CCamRotSquadEvaluator(); }
    virtual void Free() override { __super::Free(); }
};

NS_END