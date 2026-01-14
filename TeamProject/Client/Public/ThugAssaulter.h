#pragma once
#include "EnemyNormal.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CThugAssaulter final : public CEnemyNormal
{
private:
    CThugAssaulter();
    CThugAssaulter(const CThugAssaulter& rhg);
    virtual ~CThugAssaulter() = default;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    virtual void Render_GUI() override;

public:
    static CThugAssaulter* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    HRESULT Ready_Children(INIT_DESC* pArg);

public:
    /* Getter */
    CStateMachine<CThugAssaulter>*      GetStateMachine() const { return m_pStateMachine; }
    const HYSTERIESIS&                  GetHysteriesis() const { return m_tHysteriesis; }

    /* Setter */
    void                                Idle() { m_isIdle = true; }

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Ready_Rules();
    void Update_States(const _float dt);
    void ControlState(const _float dt);
    void CheckDistanceFromPlayer();

private:
    CStateMachine<CThugAssaulter>* m_pStateMachine = { nullptr };
    ATTACK_BLACK_BOARD  m_tAttackBlackBoard = {};
    HYSTERIESIS         m_tHysteriesis = {};

    _bool               m_isAutoPatternPlay = { true };

    /*For.Idle*/
    _bool               m_isIdle = { false };
    _float2             m_vIdleTime = {};

};

NS_END