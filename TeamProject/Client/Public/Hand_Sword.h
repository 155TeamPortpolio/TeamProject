#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

class CHand_Sword :
    public CEnemy
{
private:
    CHand_Sword();
    CHand_Sword(const CHand_Sword& rhg);
    virtual ~CHand_Sword() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;

    void Active_Sword();
    void Deactive_Sword();

private:

public:
    static CHand_Sword* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

};

NS_END