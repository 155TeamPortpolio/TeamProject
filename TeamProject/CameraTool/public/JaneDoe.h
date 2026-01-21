#pragma once

NS_BEGIN(CameraTool)

class JaneDoe final : public CGameObject
{
private:
    JaneDoe() DEFAULT;
    JaneDoe(const JaneDoe& rhs) : CGameObject(rhs) {}
    virtual ~JaneDoe() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override {}
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override {}

public:
    static JaneDoe* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END