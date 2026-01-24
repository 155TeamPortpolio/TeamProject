#include "pch.h"
#include "OrbitAutoYawFollowController.h"
// Engine
#include "Helper_Func.h"

void COrbitAutoYawFollowController::OnTargetChanged(const OrbitCamProfile& profile)
{
    m_state.holdTimer = profile.autoYawFollowDelay;
    m_state.hasPrevFoot = false;
    m_state.prevFoot = {};
}

_float COrbitAutoYawFollowController::EvaluateYawAddDeg(_float dt, const OrbitCamProfile& profile, const Vector3& footWorld,
    const Vector3& camLookWorld, const Vector3& camRightWorld, _float curTargetYawDeg)
{
    if (!profile.useAutoYawFollow) return 0.f;

    if (m_state.holdTimer > 0.f)
    {
        m_state.holdTimer -= dt;
        return 0.f;
    }

    if (!m_state.hasPrevFoot)
    {
        m_state.prevFoot = footWorld;
        m_state.hasPrevFoot = true;
        return 0.f;
    }

    Vector3 delta = footWorld - m_state.prevFoot;
    m_state.prevFoot = footWorld;

    delta.y = 0.f;

    const float len = delta.Length();
    if (len == 0.f) return 0.f;

    delta /= len;

    Vector3 camLook = camLookWorld;
    Vector3 camRight = camRightWorld;

    camLook.y = 0.f;
    camRight.y = 0.f;

    camLook.Normalize();
    camRight.Normalize();

    const float localZ = delta.Dot(camLook);
    const float localX = delta.Dot(camRight);

    if (localZ < 0.f && fabsf(localZ) > fabsf(localX)) return 0.f;

    const float desiredYawDeg = XMConvertToDegrees(atan2f(delta.x, delta.z));
    const float deltaYawDeg = Math::WrapDeg(desiredYawDeg - curTargetYawDeg);

    float a = 1.f - expf(-profile.autoYawFollowSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    return deltaYawDeg * a;
}