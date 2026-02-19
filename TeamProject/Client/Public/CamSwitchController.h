#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class CamSwitchController
{
public:
    enum class State { None, Enter, Hold, Switching, Recover };

    struct SwitchTuning
    {
        struct Common
        {
            _float   zoomInSec = 0.5f;
            _float   zoomInDeg = 30.f;
            EaseType zoomInEase = EaseType::OutCubic;
        } common;

        struct Goal
        {
            _float pivotAddY = 0.f;
            _float distDelta = 0.f;

            _float behindYawAddDeg = 180.f;
        } goal;

        struct Switch
        {
            _float   blendSec = 0.3f;
            EaseType blendEase = EaseType::OutCubic;

            _float   recoverPoseSec = 1.0f;
            EaseType recoverPoseEase = EaseType::OutCubic;

            _float   fovRecoverSec = 0.75f;
            EaseType fovRecoverEase = EaseType::OutCubic;

            _float   rollPeakDeg = 15.f;
        } sw;
    } tune;

    struct Pose
    {
        Vector3 pivotWorld{};
        _float  yawWorldDeg = 0.f;
        _float  pitchDeg = 0.f;
        _float  rollDeg = 0.f;
        _float  dist = 0.f;
    };

    struct Core
    {
        _bool  active = false;
        State  state = State::None;
        _float elapsed = 0.f;
    } core;

    struct Lens
    {
        _float fovSaved = 0.f;
        _float fovAppliedOffset = 0.f;
        _float recoverFromOffset = 0.f;
    } lens;

    struct HoldData
    {
        OBJECT_HANDLE target{};
        Pose  pose{};
        _bool valid = false;
    } hold;

    struct SwitchData
    {
        OBJECT_HANDLE target{};
        Pose from{};
        Pose switchTo{};
        Pose goal{};
        Pose recoverFrom{};
        Pose recoverTo{};
        _bool active = false;
    } sw;

public:
    void Begin();
    void Update(_float dt);
    void Switch();
    void End();

private:
    void CaptureHoldPose();
    void FollowHoldPivot();
    Pose CaptureCurPose() const;

    Pose BuildGoalPose(OBJECT_HANDLE target) const;
    void ApplyPose(const Pose& p) const;

    void ApplyFovOffset(_float desiredOffset);

private:
    _float EvalRecoverFovOffset(_float tSec) const;
    _float CalcBehindYawDeg(OBJECT_HANDLE target) const;
};

NS_END