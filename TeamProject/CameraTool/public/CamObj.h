#pragma once

#include "DebugCamData.h"
#include "GameObject.h"

NS_BEGIN(CameraTool)

enum class CamMoveConstraint
{
    Free, X, Y, Z, XY, XZ, YZ, Orbit
};

class CCamObj abstract : public CGameObject
{
protected:
    CCamObj() : CGameObject() {}
    CCamObj(const CCamObj& rhs) : CGameObject(rhs) {}
    virtual ~CCamObj() = default;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt)  override PURE;
    virtual void    Update(_float dt)           override PURE;
    virtual void    Late_Update(_float dt)      override PURE;

    virtual void    SetControlEnabled(_bool enabled) {}
    virtual void    SetMoveConstraint(CamMoveConstraint mode) {}
    virtual void    SetOrbitState(const CamOrbitState& next) {}

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) override PURE;
    virtual void Free() override;
};

NS_END