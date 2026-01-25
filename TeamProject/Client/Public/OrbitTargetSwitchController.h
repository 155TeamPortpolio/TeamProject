#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class COrbitTargetSwitchController
{
public:
    void    Reset() { m_state = {}; }

    void    BeginSwitch(const OrbitCamProfile& profile, const Vector3& holdPivotWorld);
    Vector3 EvaluateInternalOffset(_float dt, const OrbitCamProfile& profile, const Vector3& basePivotNow);

    void    Capture(OrbitTargetSwitchSnapshot& out) const { out.state = m_state; }
    void    Restore(const OrbitTargetSwitchSnapshot& in) { m_state = in.state; }

private:
    OrbitTargetSwitchState m_state{};
};

NS_END