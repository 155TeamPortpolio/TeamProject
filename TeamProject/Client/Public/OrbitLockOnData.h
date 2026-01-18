#pragma once

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

struct OrbitLockOnSnapshot
{
    OrbitLockOnState      state{};
    OrbitLockOnBlendState blend{};
};

struct OrbitLockOnEvalResult
{
    _float   weight = 0.f;
    _float   yawAddDeg = 0.f;
    _bool    hasDist = false;
    _float   dist = 0.f;
    Vector3  focusPos{};
};

NS_END