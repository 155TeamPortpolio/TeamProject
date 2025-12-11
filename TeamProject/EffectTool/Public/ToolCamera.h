#pragma once
#include "GameObject.h"

NS_BEGIN(EffectTool)
class CToolCamera :
    public CGameObject
{
private:
    CToolCamera();
    CToolCamera(const CToolCamera& rhs);
    virtual ~CToolCamera()DEFAULT;
public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

private:
    _float m_fSpeed = { 150.f };
    _float m_fMouseSensitive = { 10.f };

    _float2 m_vCurrentRotate = {};
public:
    static CToolCamera* Create();
    virtual void Free() override;
    CGameObject* Clone(INIT_DESC* pArg) override;
};
NS_END