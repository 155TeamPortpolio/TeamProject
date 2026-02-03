#pragma once
#include "Base.h"

NS_BEGIN(Client)

class IInteract
{
protected:
    virtual ~IInteract() = default;

public:
    virtual void Interact(class CGameObject* pObject = nullptr) PURE;
    virtual OBJECT_HANDLE Get_InteractHandle() PURE;
};

NS_END