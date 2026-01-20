#pragma once
#include "Base.h"

class IInteract
{
protected:
    virtual ~IInteract() = default;

public:
    virtual void Interact() = 0;
};

