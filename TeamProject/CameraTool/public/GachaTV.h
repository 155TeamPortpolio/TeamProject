#pragma once

NS_BEGIN(CameraTool)

class CGachaTV : public CGameObject
{
private:
    CGachaTV() {}
    CGachaTV(const CGachaTV& rhs) : CGameObject(rhs) {}
    virtual ~CGachaTV() DEFAULT;

public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* arg) override;
    void    Awake()                    override;
    void    Priority_Update(_float dt) override{}
    void    Update(_float dt)          override{}
    void    Late_Update(_float dt)     override{}

public:
    static CGachaTV* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END