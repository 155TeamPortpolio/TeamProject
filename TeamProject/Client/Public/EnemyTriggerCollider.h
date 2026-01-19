#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CEnemyTriggerCollider : public CGameObject
{
private:
    CEnemyTriggerCollider();
    CEnemyTriggerCollider(const CEnemyTriggerCollider& rhg);
    virtual ~CEnemyTriggerCollider() = default;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    virtual void Render_GUI() override;
    virtual void OnTriggerEnter(CGameObject* pOther) override;
    virtual void OnTriggerExit(CGameObject* pOther) override;


public:
    static CEnemyTriggerCollider* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END