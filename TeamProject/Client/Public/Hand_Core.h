#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

class CHand_Core :
    public CEnemy
{
private:
    CHand_Core();
    CHand_Core(const CHand_Core& rhg);
    virtual ~CHand_Core() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;

    void Active_Hand();
    void Deactive_Hand();

private:

public:
    static CHand_Core* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

};

NS_END