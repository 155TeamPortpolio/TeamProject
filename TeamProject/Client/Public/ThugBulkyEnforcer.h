#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CThugBulkyEnforcer final : public CEnemy
{
private:
    CThugBulkyEnforcer();
    CThugBulkyEnforcer(const CThugBulkyEnforcer& rhg);
    virtual ~CThugBulkyEnforcer() = default;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

public:
    static CThugBulkyEnforcer* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    void Update_States(_float dt);

private:
    CStateMachine<CThugBulkyEnforcer>* m_pStateMachine = { nullptr };
    ATTACK_BLACK_BOARD m_AttackBlackBoard{};

};

NS_END