#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CSacrificeHand :
    public CEnemy
{
private:
    CSacrificeHand();
    CSacrificeHand(const CSacrificeHand& rhg);
    virtual ~CSacrificeHand() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

public:
    static CSacrificeHand* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

public:
    ATTACK_BLACK_BOARD& GetBlackBoard() { return m_AttackBlackBoard; }
    void Phase1Attack();
    void SetActive(_bool isActive);

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();

private:
    CStateMachine<CSacrificeHand>* m_pStateMachine = { nullptr };
    ATTACK_BLACK_BOARD m_AttackBlackBoard{};
};
NS_END
