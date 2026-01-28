#include "pch.h"
#include "OrbitCollisionDistController.h"
// Engine
#include "PhysicsSystem.h"
#include "Helper_Func.h"
#include "CharacterController.h"

OrbitCollisionDistEvalResult COrbitCollisionDistController::Evaluate(_float dt, const OrbitCamProfile& profile, PxScene* scene,
    CCharacterController* camCC, const Vector3& pivotWorld, _float wantDist, const Vector2& curRotDeg, const Vector2& targetRotDeg, _float curGoalDist)
{
    OrbitCollisionDistEvalResult out{};

    const _float allowed = ComputeAllowedDist(profile, scene, camCC, pivotWorld, wantDist, curRotDeg, targetRotDeg);

    out.allowedDist = allowed;
    out.constrained = (allowed < wantDist - 0.001f);

    out.maxYawSpeedDeg = out.constrained ? profile.maxYawSpeedDegWhenColliding : profile.maxYawSpeedDeg;
    out.maxPitchSpeedDeg = out.constrained ? profile.maxPitchSpeedDegWhenColliding : profile.maxPitchSpeedDeg;

    const _float targetDist = min(wantDist, allowed);
    const _float zoomSpeed = (targetDist < curGoalDist) ? profile.collisionZoomInSpeed : profile.collisionZoomOutSpeed;

    _float nextGoal = Math::MoveTowards(curGoalDist, targetDist, zoomSpeed * dt);
    nextGoal = clamp(nextGoal, profile.minDist, profile.maxDist);

    out.goalDist = nextGoal;
    return out;
}

_float COrbitCollisionDistController::ComputeAllowedDist(const OrbitCamProfile& profile, PxScene* scene, CCharacterController* camCC,
    const Vector3& pivotWorld, _float wantDist, const Vector2& curRotDeg, const Vector2& targetRotDeg)
{
    const _float camRadius = camCC->Get_Radius();
    const _float padding = 0.1f;
    const _float stepDeg = 4.f;

    if (!scene) return wantDist;

    const _float startYaw = curRotDeg.x;
    const _float startPitch = curRotDeg.y;

    const _float endYaw = targetRotDeg.x;
    const _float endPitch = targetRotDeg.y;

    const _float deltaYaw = Math::WrapDeg(endYaw - startYaw);
    const _float deltaPitch = endPitch - startPitch;

    const _float maxAbs = max(fabsf(deltaYaw), fabsf(deltaPitch));
    _int steps = (_int)ceilf(maxAbs / stepDeg);
    if (steps < 1)  steps = 1;
    if (steps > 12) steps = 12;

    PxSphereGeometry geom(camRadius);

    PxQueryFilterData filterData;
    filterData.flags =/* PxQueryFlag::eSTATIC |*/ PxQueryFlag::ePREFILTER;

    CRaycastFilterCallback filterCallback(camCC->Get_CollisionMask(), false);

    _float minAllowed = wantDist;

    for (_int i = 1; i <= steps; ++i)
    {
        const _float t = (_float)i / (_float)steps;

        const _float yawRad = XMConvertToRadians(startYaw + deltaYaw * t);
        const _float pitchRad = XMConvertToRadians(startPitch + deltaPitch * t);

        const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

        Vector3 dir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
        dir.Normalize();

        PxTransform posePx(PxVec3(pivotWorld.x, pivotWorld.y, pivotWorld.z));

        PxSweepBuffer hit;
        PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

        const _float sweepDist = wantDist + padding;

        const _bool ok = scene->sweep(geom, posePx, PxVec3(dir.x, dir.y, dir.z), sweepDist, hit, hitFlags, filterData, &filterCallback);
        if (!ok || !hit.hasBlock) continue;

        _float allowed = hit.block.distance - padding;
        allowed = clamp(allowed, profile.minDist, wantDist);

        if (allowed < minAllowed) minAllowed = allowed;
    }

    return minAllowed;
}