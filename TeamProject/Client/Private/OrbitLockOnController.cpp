#include "pch.h"
#include "OrbitLockOnController.h"
// Engine
#include "Helper_Func.h"

void COrbitLockOnController::Reset()
{
    m_state = {};
    m_blend = {};
    m_focusPos = {};
    m_hasFocusPos = false;
}

void COrbitLockOnController::Enter(OBJECT_HANDLE handle, _float curTargetDist, const OrbitCamProfile& profile)
{
    m_state.active = true;
    m_state.handle = handle;
    m_state.savedTargetDist = curTargetDist;
    StartBlend(true, profile);
}

void COrbitLockOnController::BeginExit(const OrbitCamProfile& profile)
{
    if (!IsActiveOrBlending()) return;

    if (!m_blend.active)
    {
        StartBlend(false, profile);
        return;
    }

    if (m_blend.entering) StartBlend(false, profile);
}

void COrbitLockOnController::ForceClear()
{
    m_state = {};
    m_blend = {};
    m_focusPos = {};
    m_hasFocusPos = false;
}

void COrbitLockOnController::Capture(OrbitLockOnSnapshot& out) const
{
    out.state = m_state;
    out.blend = m_blend;
}

void COrbitLockOnController::Restore(const OrbitLockOnSnapshot& in)
{
    m_state = in.state;
    m_blend = in.blend;
    m_focusPos = {};
    m_hasFocusPos = false;
}

void COrbitLockOnController::UpdateBlend(_float dt)
{
    if (!m_blend.active)
    {
        if (m_state.active) m_blend.weight = 1.f;
        else m_blend.weight = 0.f;
        return;
    }

    m_blend.elapsed += dt;

    _float t = m_blend.elapsed / m_blend.duration;
    if (t >= 1.f)
    {
        m_blend.active = false;

        if (m_blend.entering)
        {
            m_blend.weight = 1.f;
        }
        else
        {
            m_blend.weight = 0.f;
            m_state = {};
            m_focusPos = {};
        }
        return;
    }

    t = clamp(t, 0.f, 1.f);
    const _float e = Math::ApplyEase(m_blend.ease, t);

    if (m_blend.entering) m_blend.weight = e;
    else m_blend.weight = 1.f - e;
}

OrbitLockOnEvalResult COrbitLockOnController::Evaluate(_float dt, const OrbitCamProfile& profile, OBJECT_HANDLE playerHandle, _float curTargetYawDeg, _float curTargetDist, const function<Vector3(OBJECT_HANDLE)>& getBasePivot)
{
    OrbitLockOnEvalResult out{};

    const _float w = GetWeight();
    out.weight = w;

    if (!playerHandle.isValid())
        return out;

    const Vector3 playerPivot = getBasePivot(playerHandle);

    if (m_hasFocusPos) out.focusPos = m_focusPos;
    else out.focusPos = playerPivot;

    if (m_blend.active && !m_blend.entering)
    {
        out.hasDist = true;
        const _float k = 1.f - w;
        out.dist = curTargetDist + (m_state.savedTargetDist - curTargetDist) * k;
    }

    if (w <= 0.f)
        return out;

    if (!m_state.handle.isValid())
        return out;

    const Vector3 targetPivot = getBasePivot(m_state.handle);

    Vector3 flat = targetPivot - playerPivot;
    flat.y = 0.f;

    const _float len = flat.Length();
    if (len == 0.f)
        return out;

    flat /= len;

    const _float desiredYawDeg = XMConvertToDegrees(atan2f(flat.x, flat.z));
    const _float deltaYawDeg = Math::WrapDeg(desiredYawDeg - curTargetYawDeg);

    _float a = 1.f - expf(-profile.lockOnYawSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    out.yawAddDeg = deltaYawDeg * a * w;

    _float t = len / (len + profile.lockOnFocusDist);
    t = clamp(t, 0.f, 1.f);

    const _float focusT = profile.lockOnFocusNear + (profile.lockOnFocusFar - profile.lockOnFocusNear) * t;
    out.focusPos = Vector3::Lerp(playerPivot, targetPivot, focusT);

    m_focusPos = out.focusPos;
    m_hasFocusPos = true;

    if (profile.lockOnAutoZoom && !out.hasDist)
    {
        const _float wanted = profile.startDistance + len * profile.lockOnAutoZoomFactor;
        const _float clampedDist = clamp(wanted, profile.minDist, profile.maxDist);

        if (curTargetDist < clampedDist)
        {
            out.hasDist = true;
            out.dist = curTargetDist + (clampedDist - curTargetDist) * w;
        }
    }

    return out;
}

void COrbitLockOnController::StartBlend(_bool entering, const OrbitCamProfile& profile)
{
    m_blend.active   = true;
    m_blend.entering = entering;
    m_blend.elapsed  = 0.f;

    if (entering)
    {
        m_blend.duration = profile.lockOnBlendInSec;
        m_blend.ease     = profile.lockOnBlendInEase;
        m_blend.weight   = 0.f;
    }
    else
    {
        m_blend.duration = profile.lockOnBlendOutSec;
        m_blend.ease     = profile.lockOnBlendOutEase;
        m_blend.weight   = 1.f;
    }

    if (m_blend.duration <= 0.f) m_blend.duration = 0.0001f;
}