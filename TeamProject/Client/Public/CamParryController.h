// CamParryController.h
#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class CamParryController
{
public:
    enum class State { None, Enter, Impact, ExitBlend, WaitEnd };

    struct ShotGoal
    {
        Vector3 pivotExt{};
        _float  yawDeg = 0.f;
        _float  pitchDeg = 0.f;
        _float  rollDeg = 0.f;
        _float  dist = 0.f;
        _float  yawWeight = 1.f;
    };
    struct PivotSample
    {
        Vector3 basePivot{};
        Vector3 facePivot{};
        _bool   valid = false;
    };
    struct ParryTuning
    {
        struct Common
        {
            _float enterSec = 0.1f;
            _float impactSec = 0.3f;

            _float pitchDeg = -8.f;
            _float dist = 2.85f;

            _float angleDeg = 14.f;
            _float sideYawBiasDeg = 3.f;

            _float pivotClamp = 0.85f;
            _float pelvisMul = 0.55f;

            _float forwardOffset = 0.25f;
            _float pivotYAdd = 0.00f;

            _float minPivotAboveFootY = 0.25f;
            _float minCamAboveFootY = 0.05f;

            EaseType approachEase = EaseType::InOutSine;
            EaseType impactEase = EaseType::OutSine;
        } common;
        struct Impact
        {
            _float punchDistDelta = 1.3f;

            _float rollMaxDeg = 30.f;
            _float rollArcMul = 0.85f;

            _float endCamAboveFootY = 0.5f;
            _float targetCamYMix = 0.80f;
            _float pivotDropY = 0.12f;

            _float recoverRollSec = 0.25f;
            EaseType rollEase = EaseType::InOutSine;
            EaseType recoverRollEase = EaseType::InOutSine;

            _float recoverFovSec = 0.25f;
            EaseType recoverFovEase = EaseType::InOutSine;

            _int   fovWaveCount = 4;
            _float fovWaveAmpDeg = 1.5f;
            _float fovBiasDeg = 4.f;

            _float impactStartYawExtraDeg = 30.f;
        } impact;
    } tune;

public:
    void Reset();
    void Begin();
    void End();
    void Update(_float dt);

    _bool   IsChainReentryOpen() const;
    Vector3 GetImpactPointWorld() const { return shot.fxPointWorld; }

private:
    static PivotSample SamplePivots(OBJECT_HANDLE h, _float offsetY, _float faceYOffsetMul = 0.85f);
    static Vector3     ClampOffset(const Vector3& offset, _float maxLen);
    static Vector3     RotateYDegXZ(const Vector3& dirXZ, _float deg);
    static _float      YawFromDirXZ(const Vector3& dirXZ);
    static Quaternion  YawPitchRollQuatDeg(_float yawDeg, _float pitchDeg, _float rollDeg);
    static Vector3     OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist);

private:
    void      ApplyGoalPose_Snap(const ShotGoal& g);
    _float    CurCamYawDeg() const;
    Vector3   CurCamPosWorld() const;

    void      UpdatePivots(_float dt);
    void      ClampAboveGround(ShotGoal& g) const;

    ShotGoal  BuildBaseShot_NoLens(_int sideSign) const;
    Vector3   BasePivotWorld() const;

    void      CaptureCurAsFrom();
    void      ApplyInterpolated_Enter(const ShotGoal& a, const ShotGoal& b, _float t);
    void      ClampEnter_NoDrop(ShotGoal& g) const;

    void      CaptureCurAsImpactBase();
    ShotGoal  BuildImpactShot(_int sideSign, _float close01, _float u) const;

    void      ComputeSideFromCam();
    string    BuildParryKey() const;

    void      BuildBasis(Vector3& outFwd, Vector3& outRight) const;
    Vector3   PivotWorldFromExt(const Vector3& ext) const;
    Vector3   ExtFromPivotWorld(const Vector3& pivotWorld) const;

    _float    EvalImpactFovOffset(_float u, _float close01, _float baseFov) const;
    void      ApplyImpactFov(_float u, _float close01);

    void      BeginRecoverFov();
    void      UpdateRecoverFov(_float dt);

    _bool     IsChainParry() const;
    Vector3   BuildReturnPresetCamPos() const;
    ShotGoal  BuildExitShot_FromCamPos(const Vector3& pivotWorld, const Vector3& camPosWorld) const;

private:
    struct CoreRuntime
    {
        _bool         active = false;
        State         state = State::None;
        _float        elapsed = 0.f;
        OBJECT_HANDLE attacker{};
        _bool         beginWasChain = false;
        _float        chainRefDist = 0.f;
    };
    struct SideRuntime
    {
        _int    sideSign = 1;
        _bool   isLeft = false;
        Vector3 dirXZ = Vector3(0.f, 0.f, 1.f);
    };
    struct PivotRuntime
    {
        Vector3 aBase{};
        Vector3 aFace{};
        Vector3 tBase{};
        _bool   hasTBase = false;
        _float  enterCamY = 0.f;
    };
    struct ShotRuntime
    {
        ShotGoal shotFrom{};
        ShotGoal shotTo{};
        ShotGoal impactBase{};
        _bool    impactCaptured = false;
        ShotGoal holdShot{};
        _bool    holdActive = false;
        Vector3  fxPointWorld{};
    };
    struct WaitRuntime
    {
        string seqKey{};
        _bool  seqStarted = false;
    };
    struct LensRuntime
    {
        _float fovSaved = 0.f;
        _float fovAppliedOffset = 0.f;
        _bool  recoverFovActive = false;
        _float recoverFovElapsed = 0.f;
        _float recoverFovFrom = 0.f;
    };
    struct ExitRuntime
    {
        _bool         returnLockBlend = false;
        OBJECT_HANDLE returnLockHandle{};
        ShotGoal      exitTo{};
        Vector3       exitPivotWorld{};
        Vector3       exitCamPosTo{};
        _float        exitSec = 0.f;
        Vector3       exitPivotFrom{};
        Vector3       exitCamPosFrom{};
    };

private:
    CoreRuntime core{};
    SideRuntime side{};
    PivotRuntime piv{};
    ShotRuntime shot{};
    WaitRuntime wait{};
    LensRuntime lens{};
    ExitRuntime exit{};
};

NS_END