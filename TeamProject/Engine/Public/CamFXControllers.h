#pragma once

#include "CamFXData.h"

NS_BEGIN(Engine)

class ShakeController
{
public:
    void RegisterPresets(const CamShakePreset* presets, _uint count) { m_presets = presets; m_presetCount = count; }

public:
    void Set(_float ampDeg, _float freq, _float dur, _float fadeOutSec);
    void Add(_float ampDeg, _float freq, _float dur, _float fadeOutSec);

public:
    void Set(_uint type, _float strength = 1.f);
    void Add(_uint type, _float strength = 1.f);

public:
    void Clear(_float fadeOutSec);
    void Apply(const Quaternion& camRot, _float dt, Vector3& outWorldPosDelta, Quaternion& outRotDelta);
    void Reset();

private:
    void AddPreset(const CamShakePreset& p, _float strength);
    const CamShakePreset& GetPreset(_uint type) const { return m_presets[type]; }

private:
    vector<CamShakeInstance> m_instances{};
    _uint                    m_seed = 1u;

private:
    const CamShakePreset* m_presets{};
    _uint                 m_presetCount = 0u;
};

// ---------------------------------------------------------------------------------------------------------

class ZoomController
{
public:
    void RegisterPresets(const CamZoomPreset* presets, _uint count);

public:
    void   SetPunch(_float amountDeg, _float attackSec, _float releaseSec);
    void   AddPunch(_float amountDeg, _float attackSec, _float releaseSec);

public:
    void   Set(_uint type, _float strength = 1.f);
    void   Add(_uint type, _float strength = 1.f);

public:
    void   Clear(_float fadeOutSec);
    _float Apply(_float dt);
    void   Reset() { m_instances.clear(); }

private:
    void                 AddPreset(const CamZoomPreset& p, _float strength);
    const CamZoomPreset& GetPreset(_uint type) const { return m_presets[type]; }

private:
    vector<CamZoomInstance> m_instances{};

private:
    const CamZoomPreset* m_presets{};
    _uint                m_presetCount = 0u;
};

NS_END