#pragma once

NS_BEGIN(Engine)
class CGameObject; 
NS_END

NS_BEGIN(Client)

struct OrbitLockOnState
{
    _bool         active = false;
    OBJECT_HANDLE handle{};
    _float        savedTargetDist = 0.f;
};

struct OrbitLockOnBlendState
{
    _bool    active = false;
    _bool    entering = true;
    _float   elapsed = 0.f;
    _float   duration = 0.f;
    EaseType ease = EaseType::InOutSine;
    _float   weight = 0.f;
};

struct OrbitLockOnEvalResult
{
    _float   weight = 0.f;
    _float   yawAddDeg = 0.f;
    _bool    hasDist = false;
    _float   dist = 0.f;
    Vector3  focusPos{};
};

struct OrbitAutoYawFollowState
{
    _float  holdTimer = 0.f;
    Vector3 prevFoot{};
    _bool   hasPrevFoot = false;
};

struct OrbitCollisionDistEvalResult
{
    _float allowedDist = 0.f;
    _float goalDist = 0.f;
    _bool  constrained = false;
    _float maxYawSpeedDeg = 0.f;
    _float maxPitchSpeedDeg = 0.f;
};

struct OrbitTargetSwitchState
{
    _bool   active = false;
    _float  elapsed = 0.f;
    Vector3 holdPivotWorld{};
};

struct OrbitInputEvalResult
{
    _float yawDeltaDeg = 0.f;
    _float pitchDeltaDeg = 0.f;
    _float zoomDelta = 0.f;
};

struct OrbitCamPoseState
{
    Vector2 targetRotDeg{};
    Vector2 curRotDeg{};
    _float  wantDist{};
    _float  goalDist{};
    _float  curDist{};
    Vector3 targetPivot{};
    Vector3 curPivot{};
    Vector3 pivotInternalOffset{};
    Vector3 pivotExternalOffset{};
};

struct OrbitCamInputState
{
    _float sensitivityX = 0.1f;
    _float sensitivityY = 0.08f;
    _float zoomSpeed = 1.0f;
};

struct OrbitCamProfile
{
    _float   minDist = 0.7f;
    _float   maxDist = 6.f;

    _float   pitchMin = -40.f;
    _float   pitchMax = 50.f;

    _float   rotSmoothSpeed = 14.f;
    _float   distSmoothSpeed = 12.f;
    _float   pivotSmoothSpeed = 12.f;

    _float   offsetY = 0.f;

    _float   startDistance = 4.8f;
    _float   startPitchDeg = -20.f;
    _float   startHeightOffset = 0.85f;

    _bool    useAutoYawFollow = true;
    _float   autoYawFollowSpeed = 0.4f;
    _float   autoYawFollowDelay = 0.6f;

    _float   collisionZoomInSpeed = 12.f;
    _float   collisionZoomOutSpeed = 6.f;

    _float   targetSwitchBlendSec = 1.f;
    EaseType targetSwitchEase = EaseType::OutCubic;

    _float   lockOnYawSpeed = 22.f;

    _float   lockOnFocusNear = 0.35f;
    _float   lockOnFocusFar = 0.70f;
    _float   lockOnFocusDist = 2.5f;

    _bool    lockOnAutoZoom = true;
    _float   lockOnAutoZoomFactor = 0.35f;

    _float   lockOnBlendInSec = 0.5f;
    _float   lockOnBlendOutSec = 0.5f;
    EaseType lockOnBlendInEase = EaseType::InOutSine;
    EaseType lockOnBlendOutEase = EaseType::InOutSine;

    _float   maxYawSpeedDeg = 720.f;
    _float   maxPitchSpeedDeg = 540.f;

    _float   maxYawSpeedDegWhenColliding = 200.f;
    _float   maxPitchSpeedDegWhenColliding = 180.f;
};

struct OrbitCamSnapshot
{
    OrbitCamPoseState        pose{};
    OrbitLockOnState         lockOn{};
    OrbitLockOnBlendState    lockOnBlend{};
    OrbitAutoYawFollowState  autoYaw{};
    OrbitTargetSwitchState   targetSwitch{};
    OBJECT_HANDLE            targetHandle{};
};

NS_END