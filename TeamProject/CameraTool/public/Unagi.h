#pragma once

#include "GameObject.h"

NS_BEGIN(CameraTool)

enum class Avatar { Corin };

class CUnagi : public CGameObject
{
private:
    CUnagi() = default;
    CUnagi(const CUnagi& rhs) : CGameObject(rhs) {}
    virtual ~CUnagi() = default;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake()                     override;
    void    Priority_Update(_float dt)  override {}
    void    Update(_float dt)           override;
    void    Late_Update(_float dt)      override {}
    void    Render_GUI()                override { __super::Render_GUI(); }

public:
    void   ApplyAvatar(Avatar avatar);
    void   SetAvatar(Avatar avatar) { m_avatar = avatar; }
    Avatar GetAvatar() const { return m_avatar; }

private:
    Avatar m_avatar{};

public:
    static CUnagi* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() { __super::Free(); }
};
NS_END