#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class CamSwitchController
{
public:
    enum class State { None, Enter, Hold, Switching, Recover, CancelRecover };

    struct SwitchTuning
    {
        struct Common
        {
            _float   enterSec = 0.5f;
            _float   holdFov = 30.f;
            EaseType enterEase = EaseType::OutCubic;

            _float   maxVictimDist = 10.f;
            _float   pivotFollowLerpSpeed = 18.f;

            _float   cancelFovSec = 0.5f;
            EaseType cancelFovEase = EaseType::OutCubic;
        } common;

        struct SwitchGoal
        {
            _float behindYawAddDeg = 180.f;

            _bool  chooseNearerSidePreset = true;

            _float lookOffset = -3.25f;
            _float rightOffset = 1.f;
            _float upOffset = -0.2f;

            _float distBaseAdd = 0.f;
            _float distRatio = 0.12f;
            _float distMaxAdd = 1.75f;
        } goal;

        struct Switch
        {
            _float   camPosBlendSec = 0.4f;
            EaseType camPosBlendEase = EaseType::OutCubic;

            _float   pivotBlendSec = 0.4f;
            EaseType pivotBlendEase = EaseType::OutCubic;

            _float   fovBlendSec = 0.5f;
            EaseType fovBlendEase = EaseType::OutSine;
            _float   fovSwitchRecoverTarget = 40.f;

            _float   recoverPoseSec = 1.f;
            EaseType recoverPoseEase = EaseType::InOutCubic;
            _float   recoverCamPosAddY = 1.75f;

            _float   recoverFovSec = 1.f;
            EaseType recoverFovEase = EaseType::InOutSine;
        } sw;

        struct PivotFilter
        {
            _float velTau = 0.05f;
            _float rawTau = 0.10f;
            _float outlierDist = 0.75f;
            _float outlierVel = 15.f;
        } filter;
    } tune;

    struct Pose
    {
        Vector3 pivotWorld{};
        _float  yawWorldDeg = 0.f;
        _float  pitchDeg = 0.f;
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
        _float holdDesiredFov = 0.f;

        _float fovCommanded = 0.f;
        _bool  hasFovCommanded = false;
    } lens;

    struct BeginOrbitBaseline
    {
        Pose   pose{};
        _bool  valid = false;
    } beginOrbit;

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
        Pose          pose{};
        PivotStab     pivotStab{};
    } hold;

    struct PairData
    {
        OBJECT_HANDLE attacker{};
        OBJECT_HANDLE victim{};

        Vector3 aCenter{};
        _bool   aValid = false;

        Vector3 vCenter{};
        _bool   vValid = false;
    } pair;

    struct SwitchData
    {
        OBJECT_HANDLE target{};

        Pose    fromPose{};
        Vector3 fromCamPos{};
        Vector3 fromPivot{};

        Vector3 switchCamPosGoal{};
        Vector3 switchPivotGoal{};

        Vector3 recoverCamPosFrom{};
        Vector3 recoverPivotFrom{};

        Pose    recoverTo{};
        Pose    recoverLookBasis{};

        PivotStab pivotStab{};
        _bool     active = false;
        _bool     hasRecoverLookBasis = false;

        _int sideSign = 1;

        _float fovFrom = 0.f;
        _float fovTo = 0.f;

        _float recoverFovFrom = 0.f;
        _float recoverFovTo = 0.f;
    } sw;

    struct CancelData
    {
        _float   fovFrom = 0.f;
        _float   fovTo = 0.f;
        _float   dur = 0.f;
        EaseType ease = EaseType::OutCubic;
    } cancel;

public:
    void Begin();
    void Update(_float dt);
    void End();
    void Switch();

private:
    void EnsureAutoSwitch();
    void BeginSwitchTo(OBJECT_HANDLE newTarget);
    void BeginCancelRecover();

private:
    void CaptureBeginOrbitBaseline();
    void CaptureHoldPose();
    void FollowHoldPivot(_float dt);
    Pose CaptureCurPose() const;

    void ApplyPose(const Pose& p) const;
    void ApplyFovTarget(_float desiredFov);

    _float EvalCancelFov(_float tSec) const;
    _float EvalSwitchFov(_float tSec) const;
    _float EvalRecoverFov(_float tSec) const;

    _float CalcBehindYawDeg(OBJECT_HANDLE target) const;

private:
    Vector3 CalcCenterPivot(OBJECT_HANDLE h) const;
    void    UpdatePairPivots(_float dt);

    Vector3 BuildSwitchPivotGoal_EnemyCenter() const;
    Vector3 BuildSwitchCamPosGoal_PlayerPreset(_int sideSign) const;
    _int    ChooseSwitchSideSign() const;
    Pose    BuildRecoverPose_PlayerCenter() const;
    Pose    BuildPoseFromPivotAndCamPos(const Vector3& pivotWorld, const Vector3& camPos, const Pose& fallback) const;
};

NS_END