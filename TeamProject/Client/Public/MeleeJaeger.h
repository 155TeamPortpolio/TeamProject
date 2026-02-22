#pragma once
#include "EnemyNormal.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CMeleeJaeger_Shield;
class CMeleeJaeger final : public CEnemyNormal
{
private:
    CMeleeJaeger();
    CMeleeJaeger(const CMeleeJaeger& rhg);
    virtual ~CMeleeJaeger() = default;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    virtual void Render_GUI() override;
    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) override;
    virtual void OnPooledRelease() override;
    virtual void Parried() override;

public:
    static CMeleeJaeger* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    HRESULT Ready_Children(INIT_DESC* pArg);

public:
    /* Getter */
    CStateMachine<CMeleeJaeger>*   GetStateMachine() const { return m_pStateMachine; }
    HYSTERIESIS                 GetHysteriesis() const { return m_tHysteriesis; }
    _bool                       IsShield() const { return m_isShield; }

    /* Setter */
    void            Idle() { m_isIdle = true; }
    virtual void    TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END) override;
    void            SetIsShield(_bool is); /*{ m_isShield = is; }*/
    void            StartRoll(_float fDegree);
    void            EndRoll();

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Effects();
    HRESULT Ready_Rules();
    void Update_States(const _float dt);
    void ControlState(const _float dt);
    void CheckDistanceFromPlayer();

private:
    CStateMachine<CMeleeJaeger>* m_pStateMachine = { nullptr };
    ATTACK_BLACK_BOARD      m_tAttackBlackBoard = {};
    HYSTERIESIS             m_tHysteriesis = {};
    CMeleeJaeger_Shield*    m_pShield = {};

    _bool               m_isAutoPatternPlay = { true };

    /*For.Idle*/
    _bool               m_isIdle = { false };
    _float2             m_vIdleTime = {};

    _bool               m_isShield = { false };

    // юс╫ц
    _bool               m_isStop = { false };
};

NS_END