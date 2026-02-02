#pragma once

NS_BEGIN(CameraTool)

class CGachaProps final : public CGameObject
{
private:
    CGachaProps() DEFAULT;
    CGachaProps(const CGachaProps& rhs) : CGameObject(rhs) {}
    virtual ~CGachaProps() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake()                     override{}
    void    Priority_Update(_float dt)  override;
    void    Update(_float dt)           override;
    void    Late_Update(_float dt)      override;

private:
    HRESULT Initialize_GlobalPrototype();
    void    Add_GachaProps();

private:
    vector<WEAPON_DESC> m_ResultDesc;

public:
    static CGachaProps* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END