#pragma once

#include "CamObject.h"

NS_BEGIN(AnimTool)

class CFreeCam final : public CCamObject
{
private:
    CFreeCam() DEFAULT;
    CFreeCam(const CFreeCam& rhs) : CCamObject(rhs) {}
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

public:
    static CFreeCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END