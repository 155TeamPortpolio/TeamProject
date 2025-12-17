#pragma once

#include "CamSpawnObject.h"

NS_BEGIN(Client)

class CFreeCam final : public CCamSpawnObject<CFreeCam>
{
    friend class CCamSpawnObject<CFreeCam>;

private:
    CFreeCam() DEFAULT;
    CFreeCam(const CFreeCam& rhs) : CCamSpawnObject(rhs) {}
    virtual ~CFreeCam() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* arg)  override;

    void    Priority_Update(_float dt)  override;
    void    Update(_float dt)           override {}
    void    Late_Update(_float dt)      override {}
    void    Render_GUI()                override;

private:
    void    ApplyRotation(_float dt);
    void    SyncRotation();

private:
    _float     m_fSpeed          = 5.f;
    _float     m_fSensitivity    = 0.5f;
    _float     m_fRotSmoothSpeed = 20.f;
    _vector2   m_vRotDegTarget   = {};
    Quaternion m_qCurRot         = Quaternion::Identity;
    Quaternion m_qRotTarget      = Quaternion::Identity;
};

NS_END