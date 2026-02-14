#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class CamParryController
{
    enum class State
    {
        None, Enter, Impact, Hold, End
    };

    struct ShotGoal
    {
        Vector3 pivotExt{};
        _float  yawDeg = 0.f;
        _float  pitchDeg = 0.f;
        _float  rollDeg = 0.f;
        _float  dist = 0.f;
        _float  fov = 0.f;
        _float  yawWeight = 1.f;
        _float  baseVictimWeight = 0.f;
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
            _float enterSec = 0.3f;
            _float impactSec = 0.5f;
            //_float holdSec = 2.f;
            _float holdSec = 0.1f;

            _float pitchDeg = -8.f;
            _float dist = 2.85f;
            _float fov = 34.f;

            _float angleDeg = 14.f;
            _float sideYawBiasDeg = 3.f;

            _float pivotClamp = 0.85f;
            _float pelvisMul = 0.55f;

            _float forwardOffset = 0.25f;
            _float pivotYAdd = 0.00f;

            _float contactBias = 0.35f;
            _float aimVictimBlend = 0.35f;

            _float maxVictimDist = 3.0f;
            _float minPivotAboveFootY = 0.25f;
            _float minCamAboveFootY = 0.05f;

            EaseType approachEase = EaseType::InOutSine;
            EaseType holdEase = EaseType::InOutSine;
            EaseType impactEase = EaseType::OutCubic;
        };

        struct Impact
        {
            _float punchDistDelta = 0.75f;
            _float punchFovAdd = 10.f;
            _float rollMaxDeg = 10.f;

            _float pitchLiftDeg = 6.f;

            _int   fovPulseCount = 2;
            _float fovPulseAmp = 1.25f;

            _int   rollShakeCount = 2;
            _float rollShakeDeg = 2.0f;

            _float rollArcMul = 0.35f;

            _float endCamAboveFootY = 0.9f;
            _float targetCamYMix = 0.80f;
            _float pivotDropY = 0.12f;
        };

        struct Hold
        {
            _float riseRatio = 0.35f;

            _float risePitchLiftDeg = 10.f;
            _float riseDistAdd = 0.35f;
            _float pivotUp = 0.12f;

            _float riseYawSweepDeg = 0.f;

            _float fovRestoreMid01 = 0.35f;

            EaseType riseEase = EaseType::OutSine;
            EaseType returnEase = EaseType::InOutSine;
        };

        Common common{};
        Impact impact{};
        Hold   hold{};
    };

public:
    void Reset();
    void Begin();
    void End();
    void Update(_float dt);

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
    Vector3   BasePivotWorld(_float baseVictimWeight) const;

    void      ApplyGoalPose_Snap(const ShotGoal& g);
    _float    CurCamYawDeg() const;
    Vector3   CurCamPosWorld() const;

    _int      ChooseSideSignByCamDist() const;
    void      UpdatePivots(_float dt);
    void      ClampAboveGround(ShotGoal& g) const;

    ShotGoal  BuildBaseShot(_int sideSign) const;

    void      CaptureCurAsFrom();
    void      ApplyInterpolated(const ShotGoal& a, const ShotGoal& b, _float t);

    void      ClampEnter_NoDrop(ShotGoal& g) const;
    void      ApplyInterpolated_Enter(const ShotGoal& a, const ShotGoal& b, _float t);
    ShotGoal  BuildBaseShot_NoLens(_int sideSign) const;

    void      CaptureCurAsImpactBase();
    ShotGoal  BuildImpactShot(_int sideSign, _float close01, _float roll01, _float u) const;

    void      PrepareHold(const ShotGoal& impactEnd);
    void      ApplyHoldOrbitReturn(_float u);

private:
    _bool         m_active = false;
    State         m_state = State::None;

    _float        m_elapsed{};

    OBJECT_HANDLE m_attacker{};
    OBJECT_HANDLE m_victim{};

    _int          m_sideSign = 1;

    Vector3       m_aBase{};
    Vector3       m_aFace{};
    _bool         m_aValid = false;

    Vector3       m_vBase{};
    Vector3       m_vFace{};
    _bool         m_vValid = false;

    Vector3       m_dirXZ = Vector3(0.f, 0.f, 1.f);

    ShotGoal      m_shotFrom{};
    ShotGoal      m_shotTo{};

    ShotGoal      m_holdFrom{};
    ShotGoal      m_holdMid{};
    ShotGoal      m_holdTo{};

    Vector3       m_holdPivotFromWorld{};
    Vector3       m_holdPivotMidWorld{};
    Vector3       m_holdPivotToWorld{};

    OrbitSnapshot m_prevOrbit{};
    _bool         m_prevOrbitCaptured = false;

    _float        m_prevFov{};
    _bool         m_prevFovCaptured = false;

    _bool         m_victimBlocked = false;

    _float        m_enterCamY = 0.f;

    ShotGoal      m_impactBase{};
    Vector3       m_impactPivotWorld{};
    _bool         m_impactCaptured = false;
};

NS_END
