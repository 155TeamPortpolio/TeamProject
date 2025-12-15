#pragma once

#include "DebugCamData.h"

NS_BEGIN(CameraTool)

enum class CamMoveConstraint
{
    Free, X, Y, Z, XY, XZ, YZ, Orbit
};

class CCamObj abstract : public CGameObject
{
protected:
    CCamObj() : CGameObject(), transform(m_pTransform) {}
    CCamObj(const CCamObj& rhs) : CGameObject(rhs), transform(m_pTransform), game(rhs.game) {}
    virtual ~CCamObj() = default;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;

    void Priority_Update(_float dt) override PURE;
    void Update(_float dt)          override PURE;
    void Late_Update(_float dt)     override PURE;

    virtual void SetControlEnabled(_bool enabled) {}
    virtual void SetMoveConstraint(CamMoveConstraint mode) {}
    virtual void SetOrbitState(const CamOrbitState& next) {}

protected:
    CGameInstance* game{};
    CCamera* cam{};
    CamType        camType = CamType::Debug;
    CamRigType     rigType = CamRigType::Free;
    CTransform*& transform;

public:
    CGameObject* Clone(INIT_DESC* pArg) override PURE;
    virtual void Free() override;
};

NS_END