#pragma once

#include "CamObject.h"

NS_BEGIN(Client)

class COrbitCam final : public CCamObject
{
private:
    COrbitCam() DEFAULT;
    COrbitCam(const COrbitCam& rhs) : CCamObject(rhs) {}
    virtual ~COrbitCam() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;

    void    Priority_Update(_float dt)  override;
    void    Update(_float dt)           override {}
    void    Late_Update(_float dt)      override {}
    void    Render_GUI()                override;

public:
    void    SetTarget(CTransform* target);
    void    ClearTarget();

private:
    void    UpdateInput(_float dt);
    void    ClampTargets();
    void    SmoothStates(_float dt);

    Vector3 GetPivotPos() const;
    float   GetEffectiveDistance() const;

    void    ApplyOrbitPose();

private:
    CTransform* m_pTargetTransform{};

    _float   m_offsetY = 0.2f;

    Vector2  m_rotDegTarget{};
    Vector2  m_rotDegCur{};

    _float   m_distanceTarget  = 5.f;
    _float   m_distanceCur     = 5.f;

    _float   m_pitchMin        = -30.f;
    _float   m_pitchMax        = 60.f;
    _float   m_distanceMin     = 0.5f;
    _float   m_distanceMax     = 2.f;

    _float   m_sensitivityX    = 0.15f;
    _float   m_sensitivityY    = 0.12f;
    _float   m_zoomSpeed       = 1.0f;

    _float   m_rotSmoothSpeed  = 18.f;
    _float   m_distSmoothSpeed = 18.f;

    _bool    m_usePitchDolly      = true;
    _float   m_pitchDollyStrength = 0.25f;
    _float   m_pitchDollyStartN   = 0.75f;

public:
    static COrbitCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END