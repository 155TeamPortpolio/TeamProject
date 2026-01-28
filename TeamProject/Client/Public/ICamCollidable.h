#pragma once
#include "Base.h"

class ICamCollidable
{
protected:
    virtual ~ICamCollidable() = default;

public:
    virtual void OnCameraCollision(_bool isColliding) PURE;
};

