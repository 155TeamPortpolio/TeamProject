#pragma once
#include "Base.h"

class IInteract
{
protected:
    virtual ~IInteract() = default;

public:
    virtual void Interact(class CGameObject* pObject = nullptr) PURE;
    virtual OBJECT_HANDLE Get_InteractHandle() PURE;
};

