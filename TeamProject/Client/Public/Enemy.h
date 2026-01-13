#pragma once
#include "GameObject.h"
#include "Enemy_Struct.h"

NS_BEGIN(Engine)
class CAnimator3D;
NS_END

NS_BEGIN(Client)

class CEnemy abstract:
    public CGameObject
{
public:
    enum class BATTLE_COLTYPE{ ATTACK, TRIGGER };
protected:
    CEnemy();
    CEnemy(const CEnemy& rhg);
    virtual ~CEnemy() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt) override {};
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    BATTLEOBJ_INFO*     GetCharacterOnField();
    TARGETING_INFO&     GetTargetingInfo() { return m_tTargetingInfo; }

protected:
    // Target(Player->Character)과의 거리 정보 계산
    void                ComputeTargetingInfo();
    // Target(Player->Character)이 있을 때, Target의 정보와 Target으로 부터의 정보 GUI에 렌더
    void                Render_GUI_ForTargetInfo();
    // 몬스터 공격 시 attack sign 이펙트 활성화 함수
    virtual void        Active_AttackSign() {};

protected:
#pragma region BattleCollider
    HRESULT             AttachBattleColliderObject(BATTLE_COLLIDER_DESC* pDesc);
    void                SetBattleColliderObject(const string& tagBattleColliderObject, BATTLE_COLTYPE eBattleColliderType, _bool is);
    void                FinishBattleColliderObject(const string& tagBattleColliderObject);

    void                ShowBattleColliderForCheck(_bool is);

    _bool               IsAliveBattleColliderObject(const string& tagBattleColliderObject, BATTLE_COLTYPE eBattleColliderType);

    unordered_map<string, _int> m_BattleColliderChildrenIndex;

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
    // BattleSystem으로 부터 얻어온 Character정보
    vector<BATTLEOBJ_INFO>  m_PlayerCharacterInfos; 
    // Target(Player-Character)이 있을 때, Target 사이의 정보 구조체
    TARGETING_INFO          m_tTargetingInfo = {};
    // 플레이어를 감지하는 사거리 범위(공격용 사거리 혹은 추격용으로 사용)
    _float                  m_fDetectedRange = { 5.f };    
 
protected: 
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};
NS_END