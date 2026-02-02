#pragma once

NS_BEGIN(CameraTool)

class Corin final : public CGameObject
{
private:
	Corin() DEFAULT;
	Corin(const Corin& rhs) : CGameObject(rhs) {}
	virtual ~Corin() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake()                     override;
    void    Priority_Update(_float dt)  override {}
    void    Update(_float dt)           override;
    void    Late_Update(_float dt)      override {}

public:
    static Corin* Create();
    CGameObject*  Clone(INIT_DESC* pArg) override;
};

NS_END