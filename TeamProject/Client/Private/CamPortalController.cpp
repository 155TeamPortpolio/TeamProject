#include "pch.h"
#include "CamPortalController.h"

#include "CamDirector.h"
#include "CharacterController.h"
#include "Helper_Func.h"

Quaternion CamPortalController::YawPitchQuatDeg(_float yawDeg, _float pitchDeg)
{
    const _float yawRad = XMConvertToRadians(yawDeg);
    const _float pitchRad = XMConvertToRadians(-pitchDeg);
    return Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);
}

Vector3 CamPortalController::OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist)
{
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    return pivotWorld + backDir * dist;
}

CamPortalController::ShotGoal CamPortalController::ShotFromOrbitSnapshot(const OrbitSnapshot& s, _float fov) const
{
    ShotGoal g{};
    g.pivotWorld = s.pose.pivotCurWorld;
    g.dist = s.pose.distCur;
    g.fov = fov;
    return g;
}

CamPortalController::ShotGoal CamPortalController::CaptureCurAsShot() const
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    return ShotFromOrbitSnapshot(s, cam->Get_FOV());
}

void CamPortalController::ClampAboveGround(Vector3& pivotWorld, _float yawDeg, _float pitchDeg, _float dist) const
{
    auto playerObj = ObjectManager()->Request_Object(m_player);
    auto cc = playerObj->Get_Component<CCharacterController>();

    const Vector3 foot = cc->Get_FootPosition();

    const _float minPivotY = foot.y + tune.goal.minPivotAboveFootY;
    const _float minCamY = foot.y + tune.goal.minCamAboveFootY;

    if (pivotWorld.y < minPivotY) pivotWorld.y = minPivotY;

    const Quaternion q = YawPitchQuatDeg(yawDeg, pitchDeg);
    const Vector3 camPos = OrbitPos(pivotWorld, q, dist);

    if (camPos.y < minCamY) pivotWorld.y += (minCamY - camPos.y);
}

CamPortalController::ShotGoal CamPortalController::BuildPivotGoal(_float u) const
{
    auto portalObj = ObjectManager()->Request_Object(m_portal);
    auto portalTf = portalObj->Get_Component<CTransform>();
    const Vector3 portalPos = portalTf->Get_WorldPos();
    const Vector3 portalAim = portalPos + Vector3(0.f, tune.goal.pivotYAdd, 0.f);

    auto playerObj = ObjectManager()->Request_Object(m_player);
    auto cc = playerObj->Get_Component<CCharacterController>();

    const Vector3 foot = cc->Get_FootPosition();
    const _float half = cc->Get_HalfSize();
    const Vector3 playerAim = foot + Vector3(0.f, half * 1.5f, 0.f);

    const Vector3 center = (portalAim + playerAim) * 0.5f;
    _float radius = (portalAim - center).Length();
    const _float r2 = (playerAim - center).Length();
    if (r2 > radius) radius = r2;
    radius += tune.goal.frameRadiusMargin;

    const _float halfFovRad = XMConvertToRadians(m_from.fov * 0.5f);
    _float needDist = radius / tanf(halfFovRad);
    needDist *= tune.goal.frameDistMul;

    if (needDist < tune.goal.distMin) needDist = tune.goal.distMin;
    if (needDist < m_from.dist) needDist = m_from.dist;

    ShotGoal g{};
    g.pivotWorld = Vector3::Lerp(m_from.pivotWorld, portalAim, u);
    g.dist = Math::Lerp(m_from.dist, needDist, u);
    g.fov = m_from.fov;

    return g;
}

CamPortalController::ShotGoal CamPortalController::BuildEnterGoal(_float u) const
{
    auto portalObj = ObjectManager()->Request_Object(m_portal);
    auto portalTf = portalObj->Get_Component<CTransform>();
    const Vector3 portalPos = portalTf->Get_WorldPos();
    const Vector3 portalAim = portalPos + Vector3(0.f, tune.goal.pivotYAdd, 0.f);

    const _float t = Math::ApplyEase(tune.common.enterEase, u);

    const _float enterMin = max(tune.goal.enterDistMin, 0.f);

    const _float enterDist = max(enterMin, m_enterFrom.dist * tune.goal.distMul);
    const _float finalDist = max(enterMin, enterDist * tune.goal.pullDistMul);

    const _float finalFov = m_enterFrom.fov + tune.goal.fovAdd + tune.goal.pullFovAdd;

    ShotGoal g{};
    g.pivotWorld = portalAim;
    g.dist = Math::Lerp(m_enterFrom.dist, finalDist, t);
    g.fov = Math::Lerp(m_enterFrom.fov, finalFov, t);

    return g;
}

void CamPortalController::ApplyShot(const ShotGoal& g) const
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    Vector3 pivot = g.pivotWorld;
    ClampAboveGround(pivot, m_lockYawDeg, m_lockPitchDeg, g.dist);

    const Quaternion q = YawPitchQuatDeg(m_lockYawDeg, m_lockPitchDeg);
    const Vector3 camPos = OrbitPos(pivot, q, g.dist);

    orbit->SnapFromOrbitPose(pivot, camPos, q, g.dist);
    cam->Set_FOV(g.fov);
}

void CamPortalController::Reset()
{
    m_active = false;
    m_state = State::None;

    m_elapsed = 0.f;

    m_portal.Reset();
    m_player.Reset();

    m_prevOrbit = {};
    m_prevFov = 0.f;

    m_from = {};
    m_enterFrom = {};

    m_pivotSec = 0.f;
    m_enterSec = 0.f;

    m_lockYawDeg = 0.f;
    m_lockPitchDeg = 0.f;
}

void CamPortalController::Begin(OBJECT_HANDLE portalHandle)
{
    Reset();

    m_portal = portalHandle;
    m_player = CamDirector()->GetCurHandle();

    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    orbit->CaptureSnapshot(m_prevOrbit);
    m_prevFov = cam->Get_FOV();

    m_from = ShotFromOrbitSnapshot(m_prevOrbit, m_prevFov);

    m_lockYawDeg = m_prevOrbit.pose.rotCurDeg.x;
    m_lockPitchDeg = m_prevOrbit.pose.rotCurDeg.y;

    m_pivotSec = max(tune.common.pivotSec, 0.f);
    m_enterSec = max(tune.common.enterSec, 0.f);

    m_active = true;
    m_state = State::PivotBlend;
    m_elapsed = 0.f;

    orbit->Lock_Input();
    orbit->SwitchMode_Begin();
}

void CamPortalController::End()
{
    if (!m_active) return;

    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    orbit->SwitchMode_End();

    orbit->RestoreSnapshot(m_prevOrbit);
    cam->Set_FOV(m_prevFov);

    orbit->Unlock_Input();

    Reset();
}

void CamPortalController::Update(_float dt)
{
    if (!m_active) return;

    if (m_state == State::PivotBlend)
    {
        m_elapsed += dt;

        const _float u = (m_pivotSec > 0.f) ? Math::Clamp01(m_elapsed / m_pivotSec) : 1.f;
        const _float t = Math::ApplyEase(tune.common.pivotEase, u);

        const ShotGoal g = BuildPivotGoal(t);
        ApplyShot(g);

        if (u >= 1.f)
        {
            m_state = State::Enter;
            m_elapsed = 0.f;

            m_enterFrom = CaptureCurAsShot();
        }
        return;
    }

    if (m_state == State::Enter)
    {
        m_elapsed += dt;

        const _float u = (m_enterSec > 0.f) ? Math::Clamp01(m_elapsed / m_enterSec) : 1.f;
        const ShotGoal g = BuildEnterGoal(u);
        ApplyShot(g);

        if (u >= 1.f) End();

        return;
    }
}