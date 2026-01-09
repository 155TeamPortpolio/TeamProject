#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer_Collider final : public CGameObject
{
private:
    CThugBulkyEnforcer_Collider();
    CThugBulkyEnforcer_Collider(const CThugBulkyEnforcer_Collider& rhg);
    virtual ~CThugBulkyEnforcer_Collider() = default;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    virtual void Render_GUI() override;
    virtual void OnCollisionEnter(CGameObject* pOther) override;
    virtual void OnCollisionStay(CGameObject* pOther) override;
    virtual void OnCollisionExit(CGameObject* pOther) override;


public:
    static CThugBulkyEnforcer_Collider* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;


};

NS_END