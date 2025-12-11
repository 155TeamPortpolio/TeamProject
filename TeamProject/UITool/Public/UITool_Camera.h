#pragma once
#include "GameObject.h"

NS_BEGIN(UITool)

class CUITool_Camera final : public CGameObject
{
private:
    CUITool_Camera();
    CUITool_Camera(const CUITool_Camera& rhs);
    virtual ~CUITool_Camera()DEFAULT;

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
    static CGameObject* Create();
    virtual void Free() override;
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END