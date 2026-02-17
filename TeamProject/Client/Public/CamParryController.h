#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class CamParryController
{
public:
    enum class State
    {
        None, Enter, Impact, WaitEnd
    };

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

public:
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
        };

        struct Impact
        {
            _float punchDistDelta = 1.3f;

            _float rollMaxDeg = 30.f;
            _float rollArcMul = 0.8f;

            _float endCamAboveFootY = 0.5f;
            _float targetCamYMix = 0.80f;
            _float pivotDropY = 0.12f;

            _float recoverRollSec = 0.35f;
            EaseType rollEase = EaseType::InOutSine;
            EaseType recoverRollEase = EaseType::InOutSine;

            _float recoverFovSec = 0.35f;
            EaseType recoverFovEase = EaseType::InOutSine;

            _int   fovWaveCount = 4;
            _float fovWaveAmpDeg = 1.5f;
            _float fovBiasDeg = 4.f;

            _float impactStartYawExtraDeg = 30.f;
        };

        Common common{};
        Impact impact{};
    };

public:
    void Reset();
    void Begin();
    void End();
    void Update(_float dt);

    Vector3 GetImpactPointWorld() const { return m_fxPointWorld; }

public:
    ParryTuning tune{};

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

private:
    _bool         m_active = false;
    State         m_state = State::None;

    _float        m_elapsed{};

    OBJECT_HANDLE m_attacker{};

    _int          m_sideSign = 1;
    _bool         m_isLeft = false;

    Vector3       m_aBase{};
    Vector3       m_aFace{};
    _bool         m_aValid = false;

    Vector3       m_dirXZ = Vector3(0.f, 0.f, 1.f);

    ShotGoal      m_shotFrom{};
    ShotGoal      m_shotTo{};

    _float        m_enterCamY = 0.f;

    ShotGoal      m_impactBase{};
    Vector3       m_impactPivotWorld{};
    _bool         m_impactCaptured = false;

    string        m_waitSeqKey{};
    _bool         m_waitSeqStarted = false;

    Vector3       m_fxPointWorld{};

    ShotGoal      m_holdShot{};
    _bool         m_holdActive = false;

    _float        m_fovBase = 0.f;
    _float        m_fovSaved = 0.f;

    _bool         m_recoverFovActive = false;
    _float        m_recoverFovElapsed = 0.f;
    _float        m_recoverFovFrom = 0.f;

    _float        m_fovAppliedOffset = 0.f;
};

NS_END
