#pragma once

NS_BEGIN(CameraTool)

class Unagi final : public CGameObject
{
private:
    Unagi() DEFAULT;
    Unagi(const Unagi& rhs) : CGameObject(rhs) {}
    virtual ~Unagi() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake()                     override;
    void    Priority_Update(_float dt)  override {}
    void    Update(_float dt)           override;
    void    Late_Update(_float dt)      override {}

public:
    static Unagi* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END