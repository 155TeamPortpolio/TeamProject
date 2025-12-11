#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL IService abstract :
    public CBase
{
protected:
    virtual ~IService() DEFAULT;
};

NS_END
