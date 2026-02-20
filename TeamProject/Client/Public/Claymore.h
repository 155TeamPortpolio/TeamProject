#pragma once
#include "EnemyNormal.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CClaymore final : public CEnemyNormal
{
private:
    CClaymore();
    CClaymore(const CClaymore& rhg);
    virtual ~CClaymore() = default;

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
    static CClaymore* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    HRESULT Ready_Children(INIT_DESC* pArg);

public:
    /* Getter */
    CStateMachine<CClaymore>*   GetStateMachine() const { return m_pStateMachine; }
    HYSTERIESIS                 GetHysteriesis() const { return m_tHysteriesis; }

    /* Setter */
    void            Idle() { m_isIdle = true; }
    virtual void    TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END) override;
    void            SetTutorial(_bool is) { m_isTutorial = is; }
    void            SetTutorialMode(TUTORIAL eMode);

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Ready_Rules();
    void Update_States(const _float dt);
    void ControlState(const _float dt);
    void CheckDistanceFromPlayer();
    void ManageTutorialMode(const _float dt);

private:
    CStateMachine<CClaymore>* m_pStateMachine = { nullptr };
    ATTACK_BLACK_BOARD  m_tAttackBlackBoard = {};
    HYSTERIESIS         m_tHysteriesis = {};

    _bool               m_isAutoPatternPlay = { true };

    /* Fot. Tutorial */
    _bool               m_isTutorial = { false };
    TUTORIAL            m_eCurTutorial = {};
    _bool               m_isPrevGroggy = { false };
    _bool               m_isTutorialGroggyCool = { false };
    _float2             m_vTutorialGroggyTime = {2.f, 0.f};

    /* For.Idle */
    _bool               m_isIdle = { false };
    _float2             m_vIdleTime = {};

    /* For.Debug.DamageLog */
    deque<_float>       m_DamageLog;
    _bool               m_isAccDamage = {false};
    _float              m_fAccDamage = {};


};

NS_END

/*
코린
기본 공격 1사이클 : 13.97(풀콤보), 11(평균)
특수 공격 1사이클 : 9.14

제인도
기본 공격 1사이클 : 10.93(풀콤보).
특수 공격 1사이클 : 9.49

열광상태 기본공격 1사이클 : 11

미야비
기본 공격 1사이클 : 33.31, 31.24, 32.25
특수공격 : 8.94
차징공격 26
궁극기 : 54.32, 53.10
*/