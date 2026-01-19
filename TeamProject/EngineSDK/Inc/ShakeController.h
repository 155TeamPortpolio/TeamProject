#pragma once

#include "ShakeControllerData.h"

NS_BEGIN(Engine)

class ShakeController
{
public:
    void Set(_float ampDeg, _float freq, _float dur, _float fadeOutSec);
    void Add(_float ampDeg, _float freq, _float dur, _float fadeOutSec);

    void Set(CamShakeType type, _float strength = 1.f);
    void Add(CamShakeType type, _float strength = 1.f);

    void Clear(_float fadeOutSec);
    void Apply(const Quaternion& camRot, _float dt, Vector3& outWorldPosDelta, Quaternion& outRotDelta);
    void Reset();

private:
    void AddPreset(const CamShakePreset& p, _float strength);
    const CamShakePreset& GetPreset(CamShakeType type) const;

private:
    vector<CamShakeInstance> m_instances{};
    _uint                    m_seed = 1u;
};

NS_END
