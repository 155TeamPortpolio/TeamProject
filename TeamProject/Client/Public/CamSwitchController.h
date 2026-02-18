#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class CamSwitchController
{
public:
    enum class State { None, Enter, Hold, Switching };

    struct SwitchTuning
    {
        struct Common
        {
            _float   dtMul = 1.f;

            _float   zoomInSec = 0.08f;
            _float   zoomInDeg = 12.f;
            EaseType zoomInEase = EaseType::OutCubic;
        } common;
        struct Hold
        {
            _float breathingAmpDeg = 0.f;
            _float breathingSec = 0.85f;
        } hold;
        struct Goal
        {
            _float pivotHalfMul = 0.95f;
            _float pivotAddY = 0.f;
            _float distDelta = 0.f;
        } goal;
        struct Switch
        {
            _float   blendSec = 0.18f;
            EaseType blendEase = EaseType::InOutSine;

            _float   punchSec = 0.06f;
            _float   punchDeg = 3.f;
            EaseType punchEase = EaseType::OutCubic;

            _float   fovHoldSec = 0.05f;
            _float   fovRecoverSec = 0.14f;
            EaseType fovRecoverEase = EaseType::OutCubic;
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
    } lens;
    struct HoldData
    {
        Pose  pose{};
        _bool valid = false;
    } hold;
    struct SwitchData
    {
        OBJECT_HANDLE target{};
        Pose from{};
        Pose to{};
        _bool active = false;
    } sw;

public:
    void Begin();
    void Update(_float dt);
    void Switch(OBJECT_HANDLE target);
    void End();

public:
    _bool IsActive() const { return core.active; }
    State GetState() const { return core.state; }

public:
    void SetTuning(const SwitchTuning& t) { tune = t; }
    const SwitchTuning& GetTuning() const { return tune; }

private:
    void CaptureHoldPose();
    Pose CaptureCurPose() const;

    Pose BuildGoalPose(OBJECT_HANDLE target) const;
    void ApplyPose(const Pose& p) const;

    void ApplyFovOffset(_float desiredOffset);
    _float EvalBreathingOffset(_float tSec) const;

    _float EvalSwitchFovOffset(_float tSec) const;
    _bool IsSwitchFovDone(_float tSec) const;
};

NS_END
