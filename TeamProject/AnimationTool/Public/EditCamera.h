#pragma once
#include "GameObject.h"

NS_BEGIN(AnimTool)
class CEditCamera :
    public CGameObject
{
private:
    CEditCamera();
    CEditCamera(const CEditCamera& rhs);
    virtual ~CEditCamera()DEFAULT;
public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

private:
    _float m_fSpeed = { 10.f };
    _float m_fMouseSensitive = { 10.f };

    _float2 m_vCurrentRotate = {};
public:
    static CEditCamera* Create();
    virtual void Free() override;
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END