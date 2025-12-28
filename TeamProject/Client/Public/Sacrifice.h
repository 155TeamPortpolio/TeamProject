#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CSacrifice final :
    public CEnemy
{
    enum class PARTS { ICE, WEAPON_SWORD, WEAPON_AXE, WEAPON_ROAD, END };
private:
    CSacrifice();
    CSacrifice(const CSacrifice& rhg);
    virtual ~CSacrifice() DEFAULT;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

public:
    static CSacrifice* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

public:
    /* For State Machine */
    void ActiveSword();
    void DeactiveSword();
    void ActiveAxe();
    void DeactiveAxe();
    void Idle() { m_RequestIdle = true; }
    void Attack() { m_RequestAttack = true; }
    void Walk() { m_RequestWalk = true; }

    ATTACK_BLACK_BOARD& GetBlackBoard() { return m_AttackBlackBoard; }

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();


private:
    CStateMachine<CSacrifice>* m_pStateMachine = { nullptr };
    vector<_uint> m_PartMeshIndices;
    ATTACK_BLACK_BOARD m_AttackBlackBoard{};
    _bool m_RequestIdle = false;
    _bool m_RequestAttack = false;
    _bool m_RequestWalk = false;
};
NS_END
