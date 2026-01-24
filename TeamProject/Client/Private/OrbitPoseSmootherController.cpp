#include "pch.h"
#include "OrbitPoseSmootherController.h"

void COrbitPoseSmootherController::Smooth(_float dt, const OrbitCamProfile& profile, OrbitCamPoseState& pose) const
{
    float rot = 1.f - expf(-profile.rotSmoothSpeed * dt);
    rot = clamp(rot, 0.f, 1.f);
    pose.curRotDeg = pose.curRotDeg + (pose.targetRotDeg - pose.curRotDeg) * rot;

    float dist = 1.f - expf(-profile.distSmoothSpeed * dt);
    dist = clamp(dist, 0.f, 1.f);
    pose.curDist = pose.curDist + (pose.goalDist - pose.curDist) * dist;

    float pivot = 1.f - expf(-profile.pivotSmoothSpeed * dt);
    pivot = clamp(pivot, 0.f, 1.f);
    pose.curPivot = pose.curPivot + (pose.targetPivot - pose.curPivot) * pivot;
}