#pragma once

#include "CamObject.h"

NS_BEGIN(Client)

class CFreeCam final : public CCamObject
{
private:
    CFreeCam() = default;
    CFreeCam(const CFreeCam& rhs) : CCamObject(rhs) {}
    virtual ~CFreeCam() = default;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* arg)  override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Render_GUI()                override;

public:
    void    ApplyRotation(_float dt);

private:
    _float     m_fSpeed          = 10.f;
    _float     m_fSensitivity    = 0.5f;
    _float     m_fRotSmoothSpeed = 20.f;

    _vector2   m_vRotDegTarget   = {};
    Quaternion m_qCurRot         = Quaternion::Identity;
    Quaternion m_qRotTarget      = Quaternion::Identity;

public:
    static CFreeCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END