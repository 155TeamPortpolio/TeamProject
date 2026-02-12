#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class CamWipeOutController
{
    enum class State
    {
        None,
        Shot1_Enter, Shot1_Hold,
        Shot2_Snap, Shot2_Hold,
        Shot3_Snap, Shot3_Hold,
        Shot4_Snap, Shot4_Hold,
        End
    };

    struct ShotGoal
    {
        Vector3 pivotExt{};
        _float  yawDeg = 0.f;
        _float  pitchDeg = 0.f;
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
    struct WipeTuning
    {
        _float enterBlendShot1Sec = 1.f;
        _float snapShotSec = 0.f;

        _float holdShot1Sec = 0.5f;
        _float holdShot2Sec = 1.f;
        _float holdShot3Sec = 1.f;
        _float holdShot4Sec = 0.65f;

        EaseType approachEase = EaseType::OutCubic;

        _float pitchBaseDeg = -10.f;
        _float pitchShot1UpDeg = 8.f;
        _float pitchShot2HighDeg = -14.f;
        _float pitchShot4LevelDeg = 0.f;

        _float distClose = 2.35f;
        _float distMid = 3.05f;
        _float distShot2Far = 4.20f;
        _float distShot4Far = 5.00f;

        _float fovClose = 28.f;
        _float fovMid = 32.f;
        _float fovShot2 = 40.f;
        _float fovShot4Far = 46.f;

        _float angleShot1Deg = 28.f;
        _float angleShot2Deg = 110.f;
        _float angleShot3Deg = 35.f;
        _float angleShot4Deg = 25.f;

        _float pivotClampShot1 = 0.55f;
        _float pivotClampShot2 = 1.10f;
        _float pivotClampShot3 = 0.85f;
        _float pivotClampShot4 = 1.60f;

        _float attackerBiasShot1 = 0.18f;
        _float attackerBiasShot2 = 0.10f;
        _float attackerBiasShot3 = 0.12f;
        _float attackerBiasShot4 = 0.05f;

        _float sideYawBiasDeg = 10.f;

        _float sepMin = 0.25f;
        _float sepMax = 6.50f;

        _float holdDollyShot1 = 0.08f;
        _float holdDollyShot2 = 0.12f;
        _float holdDollyShot3 = 0.10f;
        _float holdDollyShot4 = 3.80f;

        _float holdFovPunchShot4 = 22.f;

        EaseType holdEaseShot1_3 = EaseType::InOutSine;
        EaseType holdEaseShot4 = EaseType::InQuad;

        _float pelvisMul = 0.38f;

        _float shot1YawDeltaDeg = 15.f;
        _float shot1YawWeight = 0.35f;

        _float baseVictimWeightShot2 = 0.50f;
        _float baseVictimWeightShot3 = 0.35f;
        _float baseVictimWeightShot4 = 0.15f;

        _float holdYawSweepShot4 = 15.f;

        _float fitMargin = 1.18f;
        _float frameBiasMul = 0.35f;
        _float fitMinRadius = 0.55f;

        _float shot1MinPivotAboveFootY = 0.35f;
        _float shot1MinCamAboveFootY = 0.05f;

        _float shot2FitStrength = 1.0f;
        _float shot3FitStrength = 0.55f;
        _float shot4FitStrength = 0.2f;
    };

public:
    void Reset();
    void Begin();
    void End();
    void Update(_float dt);

public:
    WipeTuning tune{};

private:
    static PivotSample SamplePivots(OBJECT_HANDLE h, _float offsetY, _float faceYOffsetMul = 0.85f);
    static Vector3     ClampOffset(const Vector3& offset, _float maxLen);
    static Vector3     RotateYDegXZ(const Vector3& dirXZ, _float deg);
    static _float      YawFromDirXZ(const Vector3& dirXZ);
    static Quaternion  YawPitchQuatDeg(_float yawDeg, _float pitchDeg);
    static Vector3     OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist);

private:
    Vector3   BasePivotWorld(_float baseVictimWeight) const;
    void      ApplyGoalPose_Snap(const ShotGoal& g);

    _bool     IsHoldState(State s) const;
    _int      ResolveSideSign() const;
    _float    CurCamYawDeg() const;

    static _float FitDistForRadius(_float radius, _float fovYDeg, _float aspect, _float margin);
    _float        FitDistForPair(const ShotGoal& g, const Vector3& aWorld, const Vector3& bWorld) const;

    void      ClampShot1AboveGround(ShotGoal& g) const;

    ShotGoal  BuildShotCommon(_int sideSign, _float angleDeg, _float pitchDeg, _float dist, _float fov, _float pivotClamp, _float attackerBias, _float baseVictimWeight, _bool useMid) const;

    ShotGoal  BuildShot1() const;
    ShotGoal  BuildShot2() const;
    ShotGoal  BuildShot3() const;
    ShotGoal  BuildShot4() const;

    void      BeginShot(const ShotGoal& to, _float enterSec, _float holdSec, _bool captureFrom);
    void      CaptureCurAsFrom();

    void      ApplyInterpolated(const ShotGoal& a, const ShotGoal& b, _float t);
    void      ApplyHold();
    void      SnapTo(const ShotGoal& g);

    void      Advance();
    void      UpdatePivots(_float dt);

private:
    _bool m_active = false;
    State m_state = State::None;

    _float m_shotElapsed = 0.f;
    _float m_enterSec = 0.f;
    _float m_holdSec = 0.f;

    OBJECT_HANDLE m_attacker{};
    OBJECT_HANDLE m_victim{};

    _int m_sideSign = 1;

    Vector3 m_aBase{};
    Vector3 m_aFace{};
    _bool   m_aValid = false;

    Vector3 m_vBase{};
    Vector3 m_vFace{};
    _bool   m_vValid = false;

    Vector3 m_dirXZ = Vector3(0.f, 0.f, 1.f);
    _float  m_sep = 0.f;

    ShotGoal m_shotFrom{};
    ShotGoal m_shotTo{};

    OrbitSnapshot m_prevOrbit{};
    _bool         m_prevOrbitCaptured = false;

    _float m_prevFov = 0.f;
    _bool  m_prevFovCaptured = false;

    ShotGoal m_holdFrom{};
    ShotGoal m_holdTo{};

    Vector3     m_shot4PivotFixed{};
    Quaternion  m_shot4RotFixed{};
    _float      m_shot4DistFrom = 0.f, m_shot4DistTo = 0.f;
    _float      m_shot4FovFrom = 0.f, m_shot4FovTo = 0.f;
    _bool       m_shot4RailActive = false;
};

NS_END
