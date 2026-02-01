#pragma once

NS_BEGIN(CameraTool)

class JaneDoe final : public CGameObject
{
private:
    JaneDoe() DEFAULT;
    JaneDoe(const JaneDoe& rhs) : CGameObject(rhs) {}
    virtual ~JaneDoe() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake()                     override;
    void    Priority_Update(_float dt)  override {}
    void    Update(_float dt)           override;
    void    Late_Update(_float dt)      override {}

public:
    static JaneDoe* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END