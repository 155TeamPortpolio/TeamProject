#pragma once
#include "Base.h"

NS_BEGIN(Client)

class ICamCollidable
{
protected:
    virtual ~ICamCollidable() = default;

public:
    virtual void OnCameraCollision(_bool isColliding) PURE;
};

NS_END