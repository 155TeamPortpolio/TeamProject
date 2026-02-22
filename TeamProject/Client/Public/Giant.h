#pragma once
#include "EnemyNormal.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CGiant final : public CEnemyNormal
{
private:
    CGiant();
    CGiant(const CGiant& rhg);
    virtual ~CGiant() = default;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    virtual void Render_GUI() override;
    virtual void Parried() override;
    virtual void SetOnAttack(_bool is, ATTACK_SIDE eSide = ATTACK_SIDE::NONE) override;

public:
    static CGiant* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    HRESULT Ready_Children(INIT_DESC* pArg);

public:
    /* Getter */
    CStateMachine<CGiant>*  GetStateMachine() const { return m_pStateMachine; }
    ATTACK_BLACK_BOARD&     GetBlackBoard() { return m_tAttackBlackBoard; }
    HYSTERIESIS             GetHysteriesis() const { return m_tHysteriesis; }
    _int                    GetAttackHistoryFront() { return m_AttackHistory.front(); }
    _int                    GetAttackCombo() { return m_iAttackCombo; }

    /* Setter */
    void            Idle() { m_isIdle = true; }
    virtual void    TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END) override;
    void            AddAttackHistoryFront(_int i) { m_AttackHistory.push_front(i); }
    void            AddAttackCombo() { m_iAttackCombo++; }
    void            ResetAttackCombo() { m_iAttackCombo = 0; }
    void            SetParryDontStop(_bool is) { m_isParryDontStop = is; }

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Effects();
    HRESULT Ready_Rules();
    void Update_States(const _float dt);
    void ControlState(const _float dt);
    void CheckDistanceFromPlayer();
    void ManageAttackHistory();


private:
    CStateMachine<CGiant>* m_pStateMachine = { nullptr };
    ATTACK_BLACK_BOARD  m_tAttackBlackBoard = {};
    HYSTERIESIS         m_tHysteriesis = {};

    _bool               m_isAutoPatternPlay = { true };
    _bool               m_isParryDontStop = { false };

    /*For.Idle*/
    _bool               m_isIdle = { false };
    _float2             m_vIdleTime = {};

    /*For.AttackAlgorithm*/
    deque<_int>		    m_AttackHistory;
    _int                m_iAttackCombo = {};
};

NS_END

/*
Attack1     ->  fPunch
움직임 = 약 2m
오른손 휘두름
패링시 Parry_Right

Attack2     ->  
움직임 = 약 11m
양손 번갈아 쿵쾅쿵쾅 앞으로 (각각 4회씩)
초반에 패링 실패하면 맞아야함. 패링 성공해도 공격이 끊기진 않음. 회피는 가능

Attack2_1   ->  fJumpLong
움직임 약 8m
높이 점프해서 내려찍기
패링 되어도 끊기지않음

Attack2_Explode ->  fMiddleRangeAttack
움직임 약 6.5m
높이 점프해서 내려찍기

Attack3
움직임 약 26m(6칸반)
오른손으로 막으며 돌진
초반에 패링 실패하면 맞아야함. 패링 성공시 Parry_Right

Attack3_HitWall
움직임 약 -6m(1칸반)
벽에 맞아서 넉백

Attack4         ->  fLeapAttack
움직임 약 10m
약 6m 도약 후 왼손 휘두르고 오른손 휘두르며 전진
패링 성공 시 Parry_Right

Attack5
움직임 약 6m
전진하면서 양손으로 감싸면서 공격
패링시 별도의 애니메이션 없음(Parried 호출 X)

Attack6_AttackBack
쓰기 애매함

Attack7, 7_Jump, 7_Revenge 중복

*/