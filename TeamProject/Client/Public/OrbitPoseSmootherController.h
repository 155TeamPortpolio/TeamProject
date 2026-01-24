#pragma once

#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class COrbitPoseSmootherController
{
public:
    void Reset() {}

public:
    void Smooth(_float dt, const OrbitCamProfile& profile, OrbitCamPoseState& pose) const;
};

NS_END