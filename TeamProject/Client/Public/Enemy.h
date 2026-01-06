#pragma once
#include "GameObject.h"

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

class CEnemy abstract:
    public CGameObject
{
protected:
    CEnemy();
    CEnemy(const CEnemy& rhg);
    virtual ~CEnemy() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt) override {};
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override {};

public:
    BATTLEOBJ_INFO*     GetCharacterOnField();
    TARGETING_INFO&     GetTargetingInfo() { return m_tTargetingInfo; }
    void                ComputeTargetingInfo();

protected:
    // Target(Player-Character)이 있을 때, Target의 정보와 Target으로 부터의 정보 GUI에 렌더
    void                Render_GUI_ForTargetInfo();

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