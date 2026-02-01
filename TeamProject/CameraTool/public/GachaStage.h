#pragma once

NS_BEGIN(CameraTool)

class CGachaStage : public CGameObject
{
public:
    enum class STAGE { BANGBOO, AVATAR, END };

private:
    CGachaStage() DEFAULT;
    CGachaStage(const CGachaStage& rhs) : CGameObject(rhs){}
    virtual ~CGachaStage() DEFAULT;

public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* arg) override;
    void    Awake()                    override;
    void    Priority_Update(_float dt) override{}
    void    Update(_float dt)          override{}
    void    Late_Update(_float dt)     override{}

public:
    static CGachaStage* Create();
    CGameObject* Clone(INIT_DESC* arg) override;
};

NS_END