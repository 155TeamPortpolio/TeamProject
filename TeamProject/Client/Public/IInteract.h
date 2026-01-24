#pragma once
#include "Base.h"

class IInteract
{
protected:
    virtual ~IInteract() = default;

public:
    virtual void Interact(class CGameObject* pObject = nullptr) = 0;
};

