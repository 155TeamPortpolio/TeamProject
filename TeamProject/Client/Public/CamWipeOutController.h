#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class CamWipeOutController
{
    enum class State
    {
        None,
        Shot1_Enter, Shot1_Hold,
        Shot2_Snap,  Shot2_Hold,
        Shot3_Snap,  Shot3_Hold,
        Shot4_Snap,  Shot4_Hold,
        End
    };

    struct ShotGoal
    {
        Vector3 pivotExt{};
        _float  yawDeg    = 0.f;
        _float  pitchDeg  = 0.f;
        _float  dist      = 0.f;
        _float  fov       = 0.f;
        _float  yawWeight = 1.f;
    };

    struct PivotSample
    {
        Vector3 basePivot{};
        Vector3 facePivot{};
        _bool   valid = false;
    };

    struct WipeTuning
    {
        static constexpr _float kEnterBlendShot1Sec = 1.f;
        static constexpr _float kSnapShotSec = 0.00f;

        static constexpr _float kHoldShot1Sec = 0.5f;
        static constexpr _float kHoldShot2Sec = 1.f;
        static constexpr _float kHoldShot3Sec = 1.f;
        static constexpr _float kHoldShot4Sec = 1.f;

        static constexpr EaseType kApproachEase = EaseType::OutCubic;

        static constexpr _float kPitchBaseDeg = -12.f;
        static constexpr _float kPitchCloseDeg = -8.f;
        static constexpr _float kPitchWideDeg = -16.f;

        static constexpr _float kDistClose = 2.2f;
        static constexpr _float kDistMid = 2.9f;
        static constexpr _float kDistWide = 3.6f;

        static constexpr _float kFovClose = 28.f;
        static constexpr _float kFovMid = 32.f;
        static constexpr _float kFovWide = 38.f;

        static constexpr _float kAngleShot1Deg = 45.f;
        static constexpr _float kAngleShot2Deg = 65.f;
        static constexpr _float kAngleShot3Deg = 35.f;
        static constexpr _float kAngleShot4Deg = 30.f;

        static constexpr _float kPivotClampShot1 = 0.65f;
        static constexpr _float kPivotClampShot2 = 0.35f;
        static constexpr _float kPivotClampShot3 = 0.55f;
        static constexpr _float kPivotClampShot4 = 0.85f;

        static constexpr _float kAttackerBiasShot1 = 0.25f;
        static constexpr _float kAttackerBiasShot2 = 0.45f;
        static constexpr _float kAttackerBiasShot3 = 0.20f;
        static constexpr _float kAttackerBiasShot4 = 0.10f;

        static constexpr _float kSideYawBiasDeg = 12.f;

        static constexpr _float kSepMin = 0.25f;
        static constexpr _float kSepMax = 6.50f;

        static constexpr _float kHoldDollyShot1 = 0.10f;
        static constexpr _float kHoldDollyShot2 = 0.12f;
        static constexpr _float kHoldDollyShot3 = 0.10f;
        static constexpr _float kHoldDollyShot4 = 0.22f;

        static constexpr _float kHoldFovPunchShot4 = 10.f;

        static constexpr EaseType kHoldEaseShot1_3 = EaseType::InOutSine;
        static constexpr EaseType kHoldEaseShot4 = EaseType::InCubic;
    };

public:
    void Reset();
    void Begin(OBJECT_HANDLE victimHandle);
    void End();
    void Update(_float dt);

private:
    static PivotSample SamplePivots(OBJECT_HANDLE h, _float offsetY, _float faceYOffsetMul = 0.85f);
    static Vector3     ClampOffset(const Vector3& offset, _float maxLen);
    static Vector3     RotateYDegXZ(const Vector3& dirXZ, _float deg);
    static _float      YawFromDirXZ(const Vector3& dirXZ);
    static Quaternion  YawPitchQuatDeg(_float yawDeg, _float pitchDeg);
    static Vector3     OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist);
    void               ApplyGoalPose_Snap(const ShotGoal& g);

private:
    _bool    IsHoldState(State s) const;
    _int     ResolveSideSign() const;

    ShotGoal BuildShotCommon(_float angleDeg, _float pitchDeg, _float dist, _float fov, _float pivotClamp, _float attackerBias) const;

    ShotGoal BuildShot1() const;
    ShotGoal BuildShot2() const;
    ShotGoal BuildShot3() const;
    ShotGoal BuildShot4() const;

    void BeginShot(const ShotGoal& to, _float enterSec, _float holdSec, _bool captureFrom);
    void CaptureCurAsFrom();

    void ApplyInterpolated(const ShotGoal& a, const ShotGoal& b, _float t);
    void ApplyHold(const ShotGoal& g);
    void SnapTo(const ShotGoal& g);

    void Advance();

private:
    _bool m_active = false;
    State m_state = State::None;

    _float m_elapsed = 0.f;

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
    _bool    m_hasShotFrom = false;

    OrbitSnapshot m_prevOrbit{};
    _bool         m_prevOrbitCaptured = false;

    _float m_prevFov = 0.f;
    _bool  m_prevFovCaptured = false;

    ShotGoal m_holdFrom{};
    ShotGoal m_holdTo{};
};

NS_END