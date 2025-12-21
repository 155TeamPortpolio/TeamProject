#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CResourceRegistry :
    public CBase
{
private:
    CResourceRegistry();
    ~CResourceRegistry();

public:
    static CResourceRegistry* Create();
    void Free() override;
};

NS_END