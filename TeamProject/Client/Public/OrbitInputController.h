#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)
class COrbitAutoYawFollowController;

class COrbitInputController
{
public:
    OrbitInputEvalResult Evaluate(_float dt, const OrbitCamProfile& profile, const OrbitCamInputState& input,_float lockOnWeight,
        _float maxYawSpeedDeg, _float maxPitchSpeedDeg, COrbitAutoYawFollowController& autoYawCtrl, _bool inputLocked);
};

NS_END