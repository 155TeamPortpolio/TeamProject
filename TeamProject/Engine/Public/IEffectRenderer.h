#pragma once
#include "Base.h"

class IEffectRenderer 
    : public CBase
{
public:
    virtual void RenderAll(ID3D11DeviceContext* pContext) PURE;
    virtual ~IEffectRenderer() = default;
};
