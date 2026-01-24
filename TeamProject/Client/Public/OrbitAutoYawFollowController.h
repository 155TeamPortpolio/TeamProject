#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class COrbitAutoYawFollowController
{
public:
    void   Reset() { m_state = {}; }

    void   OnTargetChanged(const OrbitCamProfile& profile);
    void   OnManualInput(const OrbitCamProfile& profile) { m_state.holdTimer = profile.autoYawFollowDelay; }

    void   Capture(OrbitAutoYawFollowSnapshot& out) const { out.state = m_state; }
    void   Restore(const OrbitAutoYawFollowSnapshot& in) { m_state = in.state; }

    _float EvaluateYawAddDeg(_float dt, const OrbitCamProfile& profile, const Vector3& footWorld,
        const Vector3& camLookWorld, const Vector3& camRightWorld, _float curTargetYawDeg);

private:
    OrbitAutoYawFollowState m_state{};
};

NS_END