// CamSwitchController.h
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

            _float   faceYOffsetMul = 0.6f;

            _float   maxVictimDist = 12.f;

            _float   pivotFollowLerpSpeed = 18.f;
        } common;

        struct Goal
        {
            _float pivotAddY = 0.f;
            _float behindYawAddDeg = 180.f;

            _float pairYawAddDeg = 0.f;
            _float pivotForward = 0.35f;

            _float distBaseAdd = 0.f;
            _float distRatio = 0.12f;
            _float distMaxAdd = 1.75f;
        } goal;

        struct Switch
        {
            _float   blendSec = 0.3f;
            EaseType blendEase = EaseType::OutCubic;

            _float   recoverPoseSec = 1.75f;
            EaseType recoverPoseEase = EaseType::OutCubic;

            _float   fovRecoverSec = 1.5f;
            EaseType fovRecoverEase = EaseType::OutCubic;

            _float   rollPeakDeg = 5.f;
        } sw;

        struct PivotFilter
        {
            _float velTau = 0.05f;
            _float rawTau = 0.10f;
            _float outlierDist = 0.75f;
            _float outlierVel = 15.f;
        } pivot;
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
        _float fovFrom = 0.f;
        _float recoverFromFov = 0.f;
    } lens;

    struct PivotStab
    {
        _bool   hasLast = false;
        Vector3 lastRawPivot{};
        Vector3 filteredPivot{};
        Vector3 filteredVel{};

        _float  velTau = 0.05f;
        _float  rawTau = 0.10f;
        _float  outlierDist = 0.75f;
        _float  outlierVel = 15.f;
    };

    struct HoldData
    {
        OBJECT_HANDLE target{};
        Pose  pose{};
        PivotStab pivotStab{};
        _bool valid = false;
    } hold;

    struct PivotSample
    {
        Vector3 basePivot{};
        Vector3 facePivot{};
        _bool   valid = false;
    };

    struct PairData
    {
        OBJECT_HANDLE attacker{};
        OBJECT_HANDLE victim{};

        Vector3 aBase{};
        Vector3 aFace{};
        _bool   aValid = false;

        Vector3 vBase{};
        Vector3 vFace{};
        _bool   vValid = false;
    } pair;

    struct SwitchData
    {
        OBJECT_HANDLE target{};
        Pose from{};
        Pose switchTo{};
        Pose goal{};
        Pose recoverFrom{};
        Pose recoverTo{};
        PivotStab pivotStab{};
        _bool active = false;
    } sw;

public:
    void Begin();
    void Update(_float dt);
    void End();

private:
    void EnsureAutoSwitch();
    void BeginSwitchTo(OBJECT_HANDLE newTarget);

private:
    void CaptureHoldPose();
    void FollowHoldPivot(_float dt);
    Pose CaptureCurPose() const;

    void ApplyPose(const Pose& p) const;
    void ApplyFovTarget(_float desiredFov);

    _float EvalRecoverFov(_float tSec) const;
    _float CalcBehindYawDeg(OBJECT_HANDLE target) const;

private:
    PivotSample SamplePivots(OBJECT_HANDLE h, _float offsetY, _float faceYOffsetMul) const;
    void UpdatePairPivots(_float dt);
    Pose BuildGoalPose_SimplePair() const;
};

NS_END
