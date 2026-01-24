#include "pch.h"
#include "OrbitTargetSwitchController.h"
// Engine
#include "Helper_Func.h"

void COrbitTargetSwitchController::BeginSwitch(const OrbitCamProfile& profile, const Vector3& holdPivotWorld)
{
    m_state.active = true;
    m_state.elapsed = 0.f;
    m_state.holdPivotWorld = holdPivotWorld;
}

Vector3 COrbitTargetSwitchController::EvaluateInternalOffset(_float dt, const OrbitCamProfile& profile, const Vector3& basePivotNow)
{
    if (!m_state.active) return Vector3::Zero;

    m_state.elapsed += dt;

    _float t = m_state.elapsed / profile.targetSwitchBlendSec;
    if (t >= 1.f)
    {
        m_state.active = false;
        return Vector3::Zero;
    }

    t = clamp(t, 0.f, 1.f);
    t = Math::ApplyEase(profile.targetSwitchEase, t);

    const Vector3 keepOffsetNow = m_state.holdPivotWorld - basePivotNow;
    return Vector3::Lerp(keepOffsetNow, Vector3::Zero, t);
}