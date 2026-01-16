#pragma once

NS_BEGIN(CameraTool)

enum class Avatar { Corin, JaneDoe };

class CUnagi final : public CGameObject
{
private:
    CUnagi() = default;
    CUnagi(const CUnagi& rhs) : CGameObject(rhs) {}
    virtual ~CUnagi() = default;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override {}
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override {}
    virtual void    Render_GUI()                override { __super::Render_GUI(); }

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