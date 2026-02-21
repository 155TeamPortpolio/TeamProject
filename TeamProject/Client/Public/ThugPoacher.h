#pragma once
#include "EnemyNormal.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CThugPoacher_Arrow;
class CThugPoacher final : public CEnemyNormal
{
private:
    CThugPoacher();
    CThugPoacher(const CThugPoacher& rhg);
    virtual ~CThugPoacher() = default;

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
    _float3 Get_FirePos();
    void Set_LaserTarget();
    void Active_Laser();
    void Deactive_Laser();

public:
    static CThugPoacher* Create(); 
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    HRESULT Ready_Children(INIT_DESC* pArg);
    HRESULT Ready_Arrows(_uint iNum = 1);

private:
    vector<_int>     m_ArrowsChildIndices;

public:
    /* Getter */
    CStateMachine<CThugPoacher>*    GetStateMachine() const { return m_pStateMachine; }
    HYSTERIESIS                     GetHysteriesis() const { return m_tHysteriesis; }

    /* Setter */
    void            Idle() { m_isIdle = true; }
    virtual void    TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END) override;
    void            ShootArrow();

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
    CStateMachine<CThugPoacher>* m_pStateMachine = { nullptr };
    ATTACK_BLACK_BOARD  m_tAttackBlackBoard = {};
    HYSTERIESIS         m_tHysteriesis = {};

    _bool               m_isAutoPatternPlay = { true };

    /*For.Idle*/
    _bool               m_isIdle = { false };
    _float2             m_vIdleTime = {};

    /* For Effect */
    _uint m_iShotCount{};

};

NS_END