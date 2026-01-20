#pragma once

class IInteract
{
public:
    virtual ~IInteract() = default;
    virtual void Interact() = 0;
};

