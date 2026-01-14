#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CThugBulkyEnforcer final : public CEnemy
{
public:
    enum class BATTLE_PART { LEFT, RIGHT, KNEE };
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
    virtual void Render_GUI() override;
    void    Active_AttackSign() override;

public:
    static CThugBulkyEnforcer* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    HRESULT Ready_Children(INIT_DESC* pArg);

public:
    CStateMachine<CThugBulkyEnforcer>* Get_StateMachine() { return m_pStateMachine; }
    ATTACK_BLACK_BOARD& GetBlackBoard() { return m_tAttackBlackBoard; }
    HYSTERIESIS& GetHysteriesis() { return m_tHysteriesis; }
    _int                                GetAttackHistoryFront() { return m_AttackHistory.front(); }
    _int                                GetGroggyValue() { return m_iGroggyValue; }
    _bool                               IsBattleTriggerColliderOn() { return m_isBattleTriggerOn; }
    _bool                               IsBattleAttackColliderOn() { return m_isBattleAttackOn; }

    void                                Idle() { m_isIdle = true; }
    void                                CaptureRotateDir(_float3 vTargetDir, _float fSpeed = 10.f);
    void                                AddAttackHistoryFront(_int i) { m_AttackHistory.push_front(i); }
    void                                TurnOnAttackCollider(BATTLE_PART ePart);
    void                                TurnOnTriggerCollider(BATTLE_PART ePart);
    void                                FinishWeaponCollider();
    void                                SetBattleTriggerColliderOn(_bool is) { m_isBattleTriggerOn = is; }
    void                                SetBattleAttackColliderOn(_bool is) { m_isBattleAttackOn = is; }

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Ready_Rules();
    void Update_States(const _float dt);
    void ControlState(const _float dt);
    void CheckDistanceFromPlayer();
    void RotateToPlayer(const _float dt);
    void ManageAttackHistory();
    void ManageGroggy(const _float dt);

private:
    CStateMachine<CThugBulkyEnforcer>* m_pStateMachine = { nullptr };
    ATTACK_BLACK_BOARD  m_tAttackBlackBoard = {};
    HYSTERIESIS         m_tHysteriesis = {};

    _bool               m_isAutoPatternPlay = { true };
    _bool               m_isShowBattleColliderObject = { false };

    /*For.AttackAlgorithm*/
    deque<_int>		    m_AttackHistory;
    _int                m_iAttackCombo = {};
    _bool               m_isBattleTriggerOn = { false };
    _bool               m_isBattleAttackOn = { false };

    /*For.Idle*/
    _bool               m_isIdle = { false };
    _float2             m_vIdleTime = {};

    /*For.Rotate*/
    _bool               m_isLookPlayer = { false };
    _float3             m_vDirToLookCapture = {};
    _float              m_fRotateSpeed = {};

    /*For.Groggy*/
    _int                m_iGroggyValue = {};
    _float              m_fGroggyDecreaseTime = {};
};

NS_END



/*
1. attack_1 + 위빙
2. attack_2 + 백스텝
3. attack_3 끊어서 써야함 (플라잉니킥 + 내려찍기)
4. attack_4
4. 오른쪽 위빙 + attack5_1
5. 왼쪽 위빙 + attack5_2

중간중간 위빙이나 백스텝(evade)섞임

공격 2~3회 연속으로하고 플레이어 응시하면서 살짝 걸음


ThugBulkyEnforcer_Ani_Born
등장 모션(쪼그려있다가 일어남)

ThugBulkyEnforcer_Ani_Debuff_Stun_Loop
그로기 상태로 추정

ThugBulkyEnforcer_Ani_Debuff_Stun_End
그로기 끝나는 모션(->Idle로 가면 될듯)

ThugBulkyEnforcer_Ani_Ignite
불속성 공격받을 때? 모르겠음

ThugBulkyEnforcer_Ani_Ignite_End
끝나는모션(->Idle로 가면 될듯)

ThugBulkyEnforcer_Ani_Death_Stay
뒤로 쓰러져서 죽음

ThugBulkyEnforcer_Ani_Death_Back
앞으로 쓰러져서 죽음

ThugBulkyEnforcer_Ani_Death_Front
뒤로 쓰러져서 죽음(Death_Stay와 동일하게 보임)

ThugBulkyEnforcer_Ani_Attack_01
앞으로 나오면서 오른손 강펀치

ThugBulkyEnforcer_Ani_Attack_03
앞으로 도약하면서 양손 아래로 펀치

ThugBulkyEnforcer_Ani_Attack_04
바닥 짚기? 내려찍기? 3과 이어지는 착지하는 모션으로 추정

ThugBulkyEnforcer_Ani_SideStep_L
왼쪽 대각선으로 이동하며 위빙

ThugBulkyEnforcer_Ani_Attack_02
앞으로 나오면서 오른손 살짝 어퍼컷 다음 왼손 내리꽂으면서 강펀치

ThugBulkyEnforcer_Ani_SideStep_R
오른쪽 대각선으로 이동하며 위빙

ThugBulkyEnforcer_Ani_Evade
백스텝으로 피하는 모션(공격 후 뒤로 빠질때 사용)

ThugBulkyEnforcer_Ani_Attack_05_01
시퀀스1-1. 왼쪽 대각선 사이드 위빙 후 왼손 훅

ThugBulkyEnforcer_Ani_Attack_05_02
시퀀스1-2. 오른쪽 대각선 사이드 위빙으로 파고든 후 오른손 훅 치는 척하면서 왼손 어퍼컷

ThugBulkyEnforcer_Ani_Idle
가만히 서있음

ThugBulkyEnforcer_Ani_Walk_F
앞으로 걸어옴

ThugBulkyEnforcer_Ani_Run_Start
뛰기 시작하는 모션(->Run)

ThugBulkyEnforcer_Ani_Walk_B
뒷걸음

ThugBulkyEnforcer_Ani_Run
뛰는 모션 (Run_Start 이 후 동작할 것, -> Run_End)

ThugBulkyEnforcer_Ani_Run_End
뛰는걸 급하게 멈춤 (->Idle)

ThugBulkyEnforcer_Ani_Walk_L
앞을 응시하며 허리만 돌린채 왼쪽으로 걸어감(사이드스텝)

ThugBulkyEnforcer_Ani_Walk_R
앞을 응시하며 허리만 돌린채 오른쪽으로 걸어감(사이드스텝)

ThugBulkyEnforcer_Ani_Walk_RF_LFoot
앞을 응시하며 오른쪽 대각선으로 걸어감(살짝 뒤로걷는 느낌. 왼쪽 대각선보다 느림)

ThugBulkyEnforcer_Ani_Walk_FL_RFoot
앞을 응시하며 왼쪽 대각선으로 걸어감(사이드스텝)

ThugBulkyEnforcer_Ani_Walk_FR_RFoot
앞을 응시하며 오른쪽 대각선으로 걸어감(그냥 걸어감)

ThugBulkyEnforcer_Ani_Hit_Parry_Left
왼손 공격할 때, 플레이어 패링 시, 왼쪽을 쳐냄 당함.

ThugBulkyEnforcer_Ani_Hit_Parry_Right
오른손 공격할 때, 플레이어 패링 시, 오른쪽을 쳐냄 당함.

ThugBulkyEnforcer_Ani_Hit_H_Front
양손(정면) 공격 시 뒤로 패링 당함

ThugBulkyEnforcer_Ani_Debuff_Stun_Start_Front
정면에서 공격당할 때, 그로기 시작 모션(뒤로 밀려남)

ThugBulkyEnforcer_Ani_Debuff_Stun_Start_Back
뒤에서 공격당할 때, 그로기 시작 모션(앞으로 밀려남)

ThugBulkyEnforcer_Ani_Stun_Hit_H_Front
정면에서 공격당했을 때 뒤로 밀려나면서 그로기(처음부분이 로컬원점에서 뒤로 이동해서 잘라야 할 수 있음)

ThugBulkyEnforcer_Ani_Stun_Hit_H_Back
뒤에서 공격당했을 때 앞으로 밀려나면서 그로기(처음부분이 로컬원점에서 앞으로 이동해서 잘라야 할 수 있음)

ThugBulkyEnforcer_Ani_Stun_Hit_L_Front_Up
그로기 상태에서 위로 움찔

ThugBulkyEnforcer_Ani_Stun_Hit_L_Back_Up
그로기 상태에서 앞으로 숙이면서 움찔

ThugBulkyEnforcer_Ani_Stun_Hit_Stay
그로기 상태

ThugBulkyEnforcer_Ani_Stun_Hit_L_Front_Down
그로기 상태에서 바닥을 짚음?(뭐랑 연계되는지 모르겠음)

ThugBulkyEnforcer_Ani_Stun_Hit_L_Front_Left
그로기 상태에서 왼쪽 어깨가 밀려남

ThugBulkyEnforcer_Ani_Stun_Hit_L_Front_Right
그로기 상태에서 오른쪽 어깨가 밀려남

ThugBulkyEnforcer_Ani_Stun_Hit_L_Back_Down
그로기 상태에서 뒤에서 쳤을때 (Stun_Hit_L_Front_Down이랑 뭐가 다른지 모르겠음)

ThugBulkyEnforcer_Ani_Stun_Hit_L_Back_Left
그로기 상태에서 뒤에서 쳤을때 왼쪽 어깨가 앞으로 밀려남

ThugBulkyEnforcer_Ani_Stun_Hit_L_Back_Right
그로기 상태에서 뒤에서 쳤을때 오른쪽 어깨가 앞으로 밀려남

ThugBulkyEnforcer_Ani_Hit_L_Front_Up
앞에서 맞았을때 뒤로 살짝 밀림 (바로 회복됨. 플레이어가 올려치는 공격으로 추정)

ThugBulkyEnforcer_Ani_Hit_L_Back_Up
뒤에서 맞았을때. 앞으로 살짝 밀림 (바로 회복됨. 플레이어가 올려치는 공격으로 추정)

ThugBulkyEnforcer_Ani_Hit_Stay
맞는 모션. 움찔거림

ThugBulkyEnforcer_Ani_Hit_H_Back
뒤에서 맞았을때. 앞으로 많이 밀림(바로 회복)

ThugBulkyEnforcer_Ani_Hit_L_Front_Down
앞에서 내려찍는 공격 당했을때. (뒤로 안밀림, 바로회복)

ThugBulkyEnforcer_Ani_Hit_L_Front_Left
왼쪽에서 공격 당했을때. (안밀림, 바로회복)

ThugBulkyEnforcer_Ani_Hit_L_Front_Right
오른쪽에서 공격 당했을때. (안밀림, 바로회복)

ThugBulkyEnforcer_Ani_Hit_L_Back_Down
뒤에서 공격당하고 살짝 주저앉았다 일어남 (안밀림, 바로회복)

ThugBulkyEnforcer_Ani_Hit_L_Back_Left
뒤에서 공격당하고 왼쪽 어깨가 밀림. 살짝 주저앉았다 일어남 (안밀림, 바로회복)

ThugBulkyEnforcer_Ani_Hit_L_Back_Right
뒤에서 공격당하고 오른쪽 어깨가 밀림. 살짝 주저앉았다 일어남 (안밀림, 바로회복)

ThugBulkyAbuser_Ani_Debuff_Stun_Start_Front
안씀

ThugBulkyAbuser_Ani_Debuff_Stun_Start_Back
안씀

ThugBulkyEnforcer_Ani_Hit_Shake
흔들음(행동 중간에 섞는걸로 추정)

*/