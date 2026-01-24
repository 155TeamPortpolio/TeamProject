#include "pch.h"
#include "OrbitInputController.h"
#include "OrbitAutoYawFollowController.h"
// Engine
#include "GameInstance.h"

OrbitInputEvalResult COrbitInputController::Evaluate(_float dt, const OrbitCamProfile& profile, const OrbitCamInputState& input,
    _float lockOnWeight, _float maxYawSpeedDeg, _float maxPitchSpeedDeg,
    COrbitAutoYawFollowController& autoYawCtrl)
{
    OrbitInputEvalResult out{};

#ifdef _USING_GUI
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()) return out;
#endif

    const float dx = InputDevice()->Mouse_DeltaX();
    const float dy = InputDevice()->Mouse_DeltaY();

    if (lockOnWeight <= 0.f) out.yawDeltaDeg = dx * input.sensitivityX;
    out.pitchDeltaDeg = dy * input.sensitivityY;

    const float wheel = InputDevice()->Mouse_DeltaW() * 0.5f;
    if (wheel != 0.f) out.zoomDelta = wheel * input.zoomSpeed;

    if (dx != 0.f || dy != 0.f || wheel != 0.f) autoYawCtrl.OnManualInput(profile);

    const float maxYawThisFrame = maxYawSpeedDeg * dt;
    const float maxPitchThisFrame = maxPitchSpeedDeg * dt;

    out.yawDeltaDeg = clamp(out.yawDeltaDeg, -maxYawThisFrame, maxYawThisFrame);
    out.pitchDeltaDeg = clamp(out.pitchDeltaDeg, -maxPitchThisFrame, maxPitchThisFrame);

    return out;
}