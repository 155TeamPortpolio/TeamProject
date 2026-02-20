#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class CamPortalController
{
public:
    enum class State { None, PivotBlend, Enter };

    struct ShotGoal
    {
        Vector3 pivotWorld{};
        _float  yawDeg = 0.f;
        _float  pitchDeg = 0.f;
        _float  dist = 0.f;
        _float  fov = 0.f;
        _float  yawWeight = 0.f;
    };

    struct PortalTuning
    {
        struct Common
        {
            _float   pivotSec = 0.8f;
            _float   enterSec = 0.5f;

            EaseType pivotEase = EaseType::InOutSine;
            EaseType enterEase = EaseType::InCubic;
        } common;

        struct Goal
        {
            _float pitchDeg = -6.f;

            _float distMul = 0.62f;
            _float distMin = 0.5f;

            _float pullDistMul = 0.80f;

            _float fovAdd = 16.f;
            _float pullFovAdd = 6.f;

            _float pivotYAdd = 0.25f;

            _float framePlayerBias = 0.35f;
            _float frameRadiusMargin = 0.35f;
            _float frameDistMul = 1.05f;

            _float minPivotAboveFootY = 0.20f;
            _float minCamAboveFootY = 0.05f;
        } goal;
    } tune;

public:
    void  Reset();
    void  Begin(OBJECT_HANDLE portalHandle);
    void  End();
    void  Update(_float dt);

    _bool IsActive() const { return m_active; }

private:
    ShotGoal CaptureCurAsShot() const;
    ShotGoal ShotFromOrbitSnapshot(const OrbitSnapshot& s, _float fov) const;

    ShotGoal BuildPivotGoal(_float u) const;
    ShotGoal BuildEnterGoal(_float u) const;

    void ApplyShot(const ShotGoal& g) const;

    void ClampAboveGround(Vector3& pivotWorld, _float yawDeg, _float pitchDeg, _float dist) const;

    static Vector3    SafeDirXZ(const Vector3& v, const Vector3& fallback);
    static _float     YawFromDirXZ(const Vector3& dirXZ);
    static Quaternion YawPitchQuatDeg(_float yawDeg, _float pitchDeg);
    static Vector3    OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist);

private:
    _bool  m_active = false;
    State  m_state = State::None;

    _float m_elapsed = 0.f;

    OBJECT_HANDLE m_portal{};
    OBJECT_HANDLE m_player{};

    OrbitSnapshot m_prevOrbit{};
    _float        m_prevFov = 0.f;

    ShotGoal      m_from{};
    ShotGoal      m_enterFrom{};

    _float        m_startYawDeg = 0.f;

    _float        m_pivotSec = 0.f;
    _float        m_enterSec = 0.f;
    _float        m_pivotYawGoalDeg = 0.f;
};

NS_END