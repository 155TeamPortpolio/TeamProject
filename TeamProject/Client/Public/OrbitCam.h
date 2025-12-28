#pragma once

#include "CamObject.h"
#include "OrbitCamData.h"

NS_BEGIN(Client)

class COrbitCam final : public CCamObject
{
private:
    COrbitCam() = default;
    COrbitCam(const COrbitCam& rhs) : CCamObject(rhs) {}
    virtual ~COrbitCam() = default;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override {}
    virtual void    Late_Update(_float dt)      override {}
    virtual void    Render_GUI()                override { __super::Render_GUI(); }

public:
    void    SetPreset(OrbitPreset preset, _bool keepZoomRatio, _bool snap);
    void    SetTarget(CGameObject* obj);
    void    ClearTarget() { m_targetHandle.Reset(); }
    void    SyncFromCurTransform();
    void    SetTargetFrontView(CGameObject* obj, float distance, float pitchDeg, float heightOffset);

    void    SnapFromCamPose(const Vector3& camPos, const Quaternion& camRot);

private:
    void    UpdateInput(_float dt);
    void    ClampTargets();
    void    SmoothStates(_float dt);

    Vector3 GetPivotPos()          const { return m_curPivot; }
    Vector3 GetPivotTargetPos()    const;
    float   GetEffectiveDist() const;

    void    ApplyOrbitPose(_float dt);

private:
    OrbitPreset   m_preset = OrbitPreset::Field;
    _bool         m_firstSnap = true;
    OBJECT_HANDLE m_targetHandle{};

    OrbitProfile           m_profile{};
    OrbitCollisionProfile  m_colProfile{};
    OrbitCollisionState    m_colState{};

    Vector2  m_targetRotDeg{};
    Vector2  m_curRotDeg{};

    _float   m_targetDist = 2.f;
    _float   m_curDist    = 2.f;

    _float   m_sensitivityX = 0.12f;
    _float   m_sensitivityY = 0.08f;
    _float   m_zoomSpeed    = 1.0f;

    _bool    m_usePitchDolly      = true;
    _float   m_pitchDollyStrength = 0.25f;
    _float   m_pitchDollyStartN   = 0.75f;

    _float   m_targetPitchZoomOffset = 0.f;
    _float   m_curPitchZoomOffset    = 0.f;

    Vector3  m_targetPivot{};
    Vector3  m_curPivot{};

public:
    static COrbitCam* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END