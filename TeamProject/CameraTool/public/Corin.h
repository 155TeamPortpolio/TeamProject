#pragma once

NS_BEGIN(CameraTool)

class Corin final : public CGameObject
{
private:
	Corin() DEFAULT;
	Corin(const Corin& rhs) : CGameObject(rhs) {}
	virtual ~Corin() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override {}
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override {}

public:
    static Corin* Create();
    CGameObject*  Clone(INIT_DESC* pArg) override;
};

NS_END