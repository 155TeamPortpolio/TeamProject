#pragma once
#include "GameObject.h"
#include "Enemy_Struct.h"

NS_BEGIN(Engine)
class CAnimator3D;
NS_END

NS_BEGIN(Client)

enum class DIR {F, FR, R, BR, B, BL, L, FL};

class CEnemy abstract :
    public CGameObject
{
public:
    typedef struct tagEnemyCreateDesc : public Engine::GAMEOBJECT_DESC 
    {
        _float  iMaxHP = {};
    }ENEMY_DESC;

    enum class BATTLE_COLTYPE { ATTACK, TRIGGER };
protected:
    CEnemy();
    CEnemy(const CEnemy& rhg);
    virtual ~CEnemy() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void Awake()override;
    virtual void    Priority_Update(_float dt) override {};
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

    virtual void    OnPooledAcquire(INIT_DESC* pArg = nullptr) {}	// 풀에서 꺼낼 때
    virtual void    OnPooledRelease() {}							// 풀로 돌아갈 때


public:
    /* Getter */
    // 현재 플레이 중인 캐릭터의 정보를 반환
    BATTLEOBJ_INFO*     GetCharacterOnField();
    // 현재 플레이 중인 캐릭터와의 거리정보를 반환
    TARGETING_INFO&     GetTargetingInfo() { return m_tTargetingInfo; }
    // 몬스터의 Status 구조체를 반환
    MONSTER_STATUS      GetStatus() { return m_tStatus; }
    // 몬스터의 Status 구조체 포인터를 반환
    const MONSTER_STATUS*   GetStatusPtr() const { return &m_tStatus; }
    // Groggy 상태 반환
    _bool              IsGroggy() const { return m_tStatus.isGroggy; }
    // 공격중인지 상태 반환
    _bool              IsOnAttack() const { return m_isOnAttack; }

    /* Setter*/
    // 몬스터 공격 시 attack sign 이펙트 활성화 함수
    virtual void        Active_AttackSign(_bool parryEnable = true);
    // 플레이어 무기에서 몬스터한테 데미지 입힐 때 호출 될 함수
    virtual void        TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage) {};
    /* 트리거 콜라이더를 바로 키고, AttackOffsetTime 뒤에 Attack 콜라이더를
    AttackPlayTime만큼 키고 트리거와 Attack콜라이더를 종료함*/
    void                SetAutoPlayBattleCollider(const string& tagBattleCollider, _float fAttackOffsetTime, _float fAttackPlayTime, const HitDesc& hitDesc);
    /* 몬스터가 죽는 시퀀스가 다 끝나고 호출할 것. */
    void                Death();
    void                SetOnAttack(_bool is) { m_isOnAttack = is; }

protected:
    // Target(Player->Character)과의 거리 정보 계산
    void                ComputeTargetingInfo();
    // Target(Player->Character)이 있을 때, Target의 정보와 Target으로 부터의 정보 GUI에 렌더
    void                Render_GUI_ForTargetInfo();
    // Attack Sign 객체 추가 및 부착 본 지정
    virtual void        Create_AttackSign(string boneTag);
    // BattleCollider 객체 추가. 독립된 공격용 BattleCollider가 필요할 때, isSeparate = true할것(ex 레이저, 총알 등)
    HRESULT             AttachBattleColliderObject(BATTLE_COLLIDER_DESC* pDesc, _bool isSeparate = false);
    // Groggy 수치 관리
    void                ManageGroggy(const _float dt);
    // 몬스터 Look 기준 플레이어가 어느 방향에 있는지 알려주는 함수
    DIR                 GetDIRToPlayer();

    // Enemy Status 객체 추가 및 월드 행렬, 본 로컬 행렬 포인터로 전달
    virtual void        Create_UIEnemyStatus(string boneTag);
    virtual void        Create_UIBossHUD();
    void                Create_MeshPyramid();

#pragma region BattleCollider
protected:
    void                SetBattleColliderObject(const string& tagBattleColliderObject, BATTLE_COLTYPE eBattleColliderType, _bool is, const HitDesc& hitdesc = {});
    void                FinishBattleColliderObject(const string& tagBattleColliderObject);
    void                ShowBattleColliderForCheck(_bool is);
    void                CheckAutoBattlePlay(const _float dt);

    _bool               IsAliveBattleColliderObject(const string& tagBattleColliderObject, BATTLE_COLTYPE eBattleColliderType);

    unordered_map<string, _int> m_BattleColliderChildrenIndex;
    AUTO_BATTLECOL      m_tAutoBattleCol = {};

    // For.Debug
public:
    void                SetEnterAttackHit(_bool is) { m_isEnterAttackHit = is; }
    void                SetEnterTriggerHit(_bool is) { m_isEnterTriggerHit = is; }
protected:
    void                Render_GUI_ForShowBattleColliderHit();
protected:
    _bool               m_isEnterAttackHit = { false };
    _bool               m_isEnterTriggerHit = { false };
#pragma endregion

protected:
    // Status HUD 소멸할때 UI매니저에 보내서 지워야함
    UI_HANDLE               m_hUIEnemyStatus = {};
    // BattleSystem으로 부터 얻어온 Character정보
    vector<BATTLEOBJ_INFO>  m_PlayerCharacterInfos;
    // Target(Player-Character)이 있을 때, Target 사이의 정보 구조체
    TARGETING_INFO          m_tTargetingInfo = {};
    // 플레이어를 감지하는 사거리 범위(공격용 사거리 혹은 추격용으로 사용)
    _float                  m_fDetectedRange = { 5.f };
    // 몬스터 스테이터스
    MONSTER_STATUS          m_tStatus = {};
    /* Groggy */
    //_bool                   m_isGroggy = { false };
    _float                  m_fGroggyDecreaseTime = {};

    _bool                   m_isOnAttack = { false };


    /* dissolve */
    _float m_fDissolveProgress = 0.f;
    _float m_fDissolveTilling = 1.f;

protected:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};
NS_END