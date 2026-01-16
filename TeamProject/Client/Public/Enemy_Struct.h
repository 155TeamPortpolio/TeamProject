#pragma once
#include "Engine_Defines.h"

namespace Client 
{
    typedef struct tagAttackBlackBoard
    {
        deque<string> stateQueue;
        _bool isRequestNext = false;//다음 상태가 존재 할 때 상태 전환 요청
        _bool isChainOpen = false;  //현재 상태에서 다음으로 진행 가능여부
        _bool isEnd = false;
        string currentStateTag{};
    }ATTACK_BLACK_BOARD;

    typedef struct tagHysteresis 
    {
        _float fEvadeEnter{};
        _float fComboEnter{};
        _float fComboExit{};
        _float fChaseEnter{};
        _float fChaseExit{};
    }HYSTERIESIS;

    // 플레이어와 몬스터 사이의 정보를 담은 구조체
    typedef struct tagTargetingSpatialInfo 
    {
        _vector3    vSelfPos;
        _vector3    vTargetPos;
        _vector3    vDirSelfLook;
        _vector3    vDirToTarget;   // 몬스터->플레이어를 바라보는 방향
        _float      fDotTarget;     // 1.0이면 정면, 0이면 90도(옆), -1.0이면 정반대 뒤쪽을 의미
        _float      fDistance;      // 제곱근 (실제 거리) 
        _float      fDistanceSq;    // 제곱한 거리
        _bool       isDetected = false; // Enemy-m_fDetectedRange 범위 안에 감지가 되면 true
    }TARGETING_INFO;

    
    typedef struct tagBattleColliderInitDesc 
    {
        string          tagName = "";                           // 등록될 Collider오브젝트 이름
        _bool           isAttachBone = { true };                // 뼈에 붙이는지
        string          tagBone = "";                           // 뼈에 붙일때, 붙일 뼈의 이름
        CAnimator3D* pOwnerAnimator3D = { nullptr };           // 뼈에 붙일때, Owner의 애니메이터 포인터
        COLLIDER_TYPE   eAttackColliderType = COLLIDER_TYPE::SPHERE;  // BOX, SPHERE, CAPSULE
        _float			fSizeScale = 1.f;					    // 사이즈 비율
        _float3			vCenter = { 0.f, 0.f, 0.f };		    // Collider의 로컬 오프셋
        _float3			vRotation = { 0.f, 0.f, 0.f };
        /* 실제 Hit처리될 Attack용 콜라이더의 사이즈
        Box: HalfExtents(x, y, z),
        Sphere : Radius(x),
        Capsule : Radius(x) / HalfHeight(y)*/
        _float3         vAttackSize = { 1.f, 1.f, 1.f };
        COLLIDER_TYPE   eTriggerColliderType = COLLIDER_TYPE::SPHERE;  // BOX, SPHERE, CAPSULE
        /* Parrying 및 회피용 콜라이더의 사이즈. Attack용보다 사이즈 크게 할 것
        Box: HalfExtents(x, y, z),
        Sphere : Radius(x),
        Capsule : Radius(x) / HalfHeight(y)*/
        _float3         vTriggerSize = { 2.f, 2.f, 2.f };
    }BATTLE_COLLIDER_DESC;

    typedef struct tagAutoPlayBattleCollider 
    {
        string      tagBattleCollider = "";
        _bool       isAutoPlay = { false };
        _float      fAttackColStartProgress = {};
        _bool       isAttackColliderPlay = {false};
        _float2     vAttackColLifeTime = {};
        _bool       IsAttackColFinish() { return vAttackColLifeTime.x <= vAttackColLifeTime.y; }
    }AUTO_BATTLECOL;

    typedef struct tagMonsterStatus 
    {
        _float        iHP = {};
        _int        iGroggyValue = {};
    }MONSTER_STATUS;
}