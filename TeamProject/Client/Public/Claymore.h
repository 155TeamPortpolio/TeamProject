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