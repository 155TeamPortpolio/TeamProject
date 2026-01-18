#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class COrbitLockOnController
{
public:
    void                  Reset();

    _bool                 IsActive() const { return m_state.active; }
    _bool                 IsBlending() const { return m_blend.active; }
    _bool                 IsActiveOrBlending() const { return m_state.active || m_blend.active; }

    OBJECT_HANDLE         GetHandle() const { return m_state.handle; }
    _float                GetWeight() const { return m_blend.weight; }
    Vector3               GetFocusPos() const { return m_focusPos; }
    _float                GetSavedTargetDist() const { return m_state.savedTargetDist; }

public:
    void                  Enter(OBJECT_HANDLE handle, _float curTargetDist, const OrbitCamProfile& profile);
    void                  Switch(OBJECT_HANDLE handle) { m_state.handle = handle; }
    void                  BeginExit(const OrbitCamProfile& profile);
    void                  ForceClear();

public:
    void                  Capture(OrbitLockOnSnapshot& out) const;
    void                  Restore(const OrbitLockOnSnapshot& in);

public:
    void                  UpdateBlend(_float dt);

    OrbitLockOnEvalResult Evaluate(_float dt, const OrbitCamProfile& profile, OBJECT_HANDLE playerHandle,
        _float curTargetYawDeg, _float curTargetDist, const function<Vector3(OBJECT_HANDLE)>& getBasePivot);

private:
    void                  StartBlend(_bool entering, const OrbitCamProfile& profile);

private:
    OrbitLockOnState      m_state{};
    OrbitLockOnBlendState m_blend{};
    Vector3               m_focusPos{};
    _bool                 m_hasFocusPos = false;
};

NS_END