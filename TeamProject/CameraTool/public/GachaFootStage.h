#pragma once

NS_BEGIN(CameraTool)

class CGachaFootStage : public CGameObject
{
private:
    CGachaFootStage() {}
    CGachaFootStage(const CGachaFootStage& rhs) : CGameObject(rhs) {}
    virtual ~CGachaFootStage() DEFAULT;

public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* arg) override;
    void    Awake()                    override;
    void    Priority_Update(_float dt) override {}
    void    Update(_float dt)          override {}
    void    Late_Update(_float dt)     override {}

public:
    static CGachaFootStage* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END