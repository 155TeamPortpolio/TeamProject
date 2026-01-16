#pragma once

NS_BEGIN(Engine)

struct OrbitCamPoseState
{
    Vector2 targetRotDeg{};
    Vector2 curRotDeg{};
    _float  targetDist{};
    _float  curDist{};
    Vector3 targetPivot{};
    Vector3 curPivot{};
    Vector3 pivotOverrideOffset{};
};

struct OrbitCamInputState
{
    _float sensitivityX = 0.12f;
    _float sensitivityY = 0.08f;
    _float zoomSpeed    = 1.0f;
};

struct OrbitCamProfile
{
    _float minDist = 1.f;
    _float maxDist = 6.f;

    _float pitchMin = -30.f;
    _float pitchMax = 40.f;

    _float rotSmoothSpeed   = 16.f;
    _float distSmoothSpeed  = 16.f;
    _float pivotSmoothSpeed = 14.f;

    _float offsetY = 0.f;

    _float startDistance = 4.8f;
    _float startPitchDeg = -20.f;
    _float startHeightOffset = 0.85f;

    _bool  useAutoYawFollow = true;
    _float autoYawFollowSpeed = 0.4f;
    _float autoYawFollowDelay = 0.6f;

    _float collisionZoomInSpeed  = 12.f;
    _float collisionZoomOutSpeed = 6.f;

    _float   targetSwitchBlendSec = 1.f;
    EaseType targetSwitchEase = EaseType::OutCubic;

    _float lockOnYawSpeed = 22.f;

    _float lockOnFocusNear = 0.35f;
    _float lockOnFocusFar = 0.70f;
    _float lockOnFocusDist = 2.5f;

    _bool  lockOnAutoZoom = true;
    _float lockOnAutoZoomFactor = 0.35f;
};

struct OrbitCamLockOnState
{
    _bool         active = false;
    OBJECT_HANDLE handle{};
};

struct OrbitCamTargetSwitchState
{
    _bool   active  = false;
    _float  elapsed = 0.f;
    Vector3 holdPivotWorld{};
};

struct OrbitCamSnapshot
{
    OrbitCamPoseState         pose{};
    OrbitCamTargetSwitchState targetSwitch{};
    _float                    autoYawHoldTimer = 0.f;
    Vector3                   prevTargetFoot{};
    _bool                     hasPrevTargetFoot = false;
    OBJECT_HANDLE             targetHandle{};
};

NS_END