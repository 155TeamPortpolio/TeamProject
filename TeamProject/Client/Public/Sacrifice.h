#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CSacrifice final :
    public CEnemy
{
public:
    enum class PHASE { PHASE1, PHASE2, END };
    enum class PARTS { ICE, WEAPON_SWORD, WEAPON_AXE, WEAPON_WHIP, END };

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
    void ActiveWhip();
    void DeactiveWhip();

    void SetPhase(PHASE phase) { m_eCurrPhase = phase; }
    _bool IsOverDrive()const { return m_IsOverDrive; }
    void ActiveOverDrive() { m_IsOverDrive = true; }
    void DeactiveOverDrive() { m_IsOverDrive = false; }

    void Idle();
    void Evade();
    void ChangePhase();

    ATTACK_BLACK_BOARD& GetBlackBoard() { return m_AttackBlackBoard; }
    PHASE GetCurrPhase()const { return m_eCurrPhase; }

    /* Hand */
    void Phase1Attack();

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    void Update_States(_float dt);

private:
    CStateMachine<CSacrifice>* m_pStateMachine = { nullptr };
    vector<_uint> m_PartMeshIndices;
    ATTACK_BLACK_BOARD m_AttackBlackBoard{};
    _bool m_RequestIdle = false;

    _float m_fIdleElasedTime{};
    _float m_fIdleDuration = 0.1f;
    _float m_fPhase1ElapseTime{};
    _float m_fPhase1Duration = 10.f;

    PHASE m_eCurrPhase = PHASE::PHASE1;
    _bool m_IsOverDrive = false; /* Only Use Phase2 */

    _uint m_iHandID{};

};
NS_END
