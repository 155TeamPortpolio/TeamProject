#pragma once

NS_BEGIN(Engine)
class CGameObject; class CCharacterController;
NS_END

NS_BEGIN(Client)

struct OrbitLockState
{
    OBJECT_HANDLE handle{};
    _bool         active    = false;
    _float        savedDist = 0.f;
};

struct OrbitBlendState
{
    _bool    active   = false;
    _bool    entering = true;
    _float   elapsed  = 0.f;
    _float   duration = 0.f;
    EaseType ease     = EaseType::InOutSine;
    _float   weight   = 0.f;
};

struct OrbitLockEval
{
    _float   weight    = 0.f;
    _float   yawAddDeg = 0.f;
    _bool    hasDist   = false;
    _float   dist      = 0.f;
    Vector3  focusPos{};
};

struct OrbitAutoYaw
{
    Vector3 prevFootWorld{};
    _float  holdTimer   = 0.f;
    _bool   hasPrevFoot = false;
};

struct OrbitCollideEval
{
    _float allowedDist = 0.f;
    _float goalDist    = 0.f;
    _bool  hit         = false;

    _float yawDeltaCapDeg = 0.f;
    _float pitchDeltaCapDeg = 0.f;
};

struct OrbitSwitch
{
    _bool   active  = false;
    _float  elapsed = 0.f;
    Vector3 holdPivotWorld{};
};

struct OrbitInputEval
{
    _float yawDeltaDeg   = 0.f;
    _float pitchDeltaDeg = 0.f;
    _float zoomDelta     = 0.f;
};

struct OrbitPose
{
    Vector2 rotGoalDeg{};
    Vector2 rotCurDeg{};

    _float  distWanted = 0.f;
    _float  distGoal   = 0.f;
    _float  distCur    = 0.f;

    Vector3 pivotGoalWorld{};
    Vector3 pivotCurWorld{};

    Vector3 pivotInternalOffset{};
    Vector3 pivotExternalOffset{};
};

struct OrbitInput
{
    _float sensX     = 0.1f;
    _float sensY     = 0.08f;
    _float zoomSpeed = 1.0f;
};

struct OrbitProfile
{
    _float distMin = 0.7f;
    _float distMax = 6.f;

    _float pitchLimitMinDeg = -40.f;
    _float pitchLimitMaxDeg = 50.f;

    _float rotSmooth   = 14.f;
    _float distSmooth  = 12.f;
    _float pivotSmooth = 12.f;

    _float offsetY     = 0.f;

    _float startDist     = 4.8f;
    _float startPitchDeg = -20.f;
    _float startHeight   = 0.85f;

    _bool  autoYaw      = true;
    _float autoYawSpeed = 0.4f;
    _float autoYawDelay = 0.6f;

    _float zoomInCollide  = 12.f;
    _float zoomOutCollide = 6.f;

    _float switchBlendSec = 1.f;
    EaseType switchEase   = EaseType::OutCubic;

    _float lockYawSpeed = 22.f;

    _float lockFocusNear = 0.35f;
    _float lockFocusFar  = 0.70f;
    _float lockFocusDist = 2.5f;

    _bool  lockAutoZoom = true;
    _float lockAutoZoomFactor = 0.35f;

    _float lockBlendIn = 0.5f;
    _float lockBlendOut = 0.5f;
    EaseType lockBlendInEase = EaseType::InOutSine;
    EaseType lockBlendOutEase = EaseType::InOutSine;

    _float yawDeltaCapDeg = 720.f;
    _float pitchDeltaCapDeg = 540.f;

    _float yawHitDeltaCapDeg = 200.f;
    _float pitchHitDeltaCapDeg = 180.f;
};

struct OrbitSnapshot
{
    OrbitPose       pose{};
    OrbitLockState  lock{};
    OrbitBlendState lockBlend{};
    OrbitAutoYaw    autoYaw{};
    OrbitSwitch     sw{};
    OBJECT_HANDLE   target{};
};

struct OrbitPivotStabilizer
{
    Vector3 lastRawPivot{};
    Vector3 rawVel{};
    _bool   hasLast = false;

    Vector3 filteredPivot{};
    Vector3 filteredVel{};

    _float  rawTau = 0.06f;
    _float  velTau = 0.10f;
    _float  outlierDist = 0.25f;
    _float  outlierVel = 7.0f;
};

NS_END
