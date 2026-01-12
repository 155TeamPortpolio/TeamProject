#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CAnimator3D;
NS_END

NS_BEGIN(Client)
typedef struct tagAttackBlackBoard
{
    deque<string> stateQueue;
    _bool isRequestNext = false;//다음 상태가 존재 할 때 상태 전환 요청
    _bool isChainOpen = false;  //현재 상태에서 다음으로 진행 가능여부
    _bool isEnd = false;
    string currentStateTag{};
}ATTACK_BLACK_BOARD;

// 플레이어와 몬스터 사이의 정보를 담은 구조체
typedef struct tagTargetingSpatialInfo {
    _vector3    vSelfPos;
    _vector3    vTargetPos;
    _vector3    vDirSelfLook;
    _vector3    vDirToTarget;   // 몬스터->플레이어를 바라보는 방향
    _float      fDotTarget;     // 1.0이면 정면, 0이면 90도(옆), -1.0이면 정반대 뒤쪽을 의미
    _float      fDistance;      // 제곱근 (실제 거리) 
    _float      fDistanceSq;    // 제곱한 거리
    _bool       isDetected = false; // Enemy-m_fDetectedRange 범위 안에 감지가 되면 true
}TARGETING_INFO;

// Bone에 붙일 
typedef struct tagBattleColliderInitDesc {
    string          tagName = "";                           // 등록될 Collider오브젝트 이름
    _bool           isAttachBone = { true };                // 뼈에 붙이는지
    string          tagBone = "";                           // 뼈에 붙일때, 붙일 뼈의 이름
    CAnimator3D*    pOwnerAnimator3D = {nullptr};           // 뼈에 붙일때, Owner의 애니메이터 포인터
    COLLIDER_TYPE   eColliderType = COLLIDER_TYPE::SPHERE;  // BOX, SPHERE, CAPSULE
    _float			fSizeScale = 1.f;					    // 사이즈 비율
    _float3			vCenter = { 0.f, 0.f, 0.f };		    // Collider의 로컬 오프셋
    _float3			vRotation = { 0.f, 0.f, 0.f };
    /* 실제 Hit처리될 Attack용 콜라이더의 사이즈
    Box: HalfExtents(x, y, z), 
    Sphere : Radius(x),
    Capsule : Radius(x) / HalfHeight(y)*/ 
    _float3         vAttackSize = { 1.f, 1.f, 1.f };       
    /* Parrying 및 회피용 콜라이더의 사이즈. Attack용보다 사이즈 크게 할 것
    Box: HalfExtents(x, y, z), 
    Sphere : Radius(x),
    Capsule : Radius(x) / HalfHeight(y)*/
    _float3         vTriggerSize = { 2.f, 2.f, 2.f };        
}BATTLE_COLLIDER_DESC;

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
    void                ComputeTargetingInfo();

protected:
    // Target(Player-Character)이 있을 때, Target의 정보와 Target으로 부터의 정보 GUI에 렌더
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