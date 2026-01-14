#pragma once

NS_BEGIN(CameraTool)

class CAvatar final : public CGameObject
{
private:
	CAvatar() DEFAULT;
	CAvatar(const CAvatar& rhs) : CGameObject(rhs) {}
	virtual ~CAvatar() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

private:


public:
    static  CAvatar* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END