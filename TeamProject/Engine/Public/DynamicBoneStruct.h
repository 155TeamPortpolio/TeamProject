#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

typedef struct DynamicBoneNode 
{
    _int BoneIndex = { -1 };
    _int ParentIndex = { -1 };

    // (부모가 원점 -> 부모원점기준 나) 부모로컬로부터 나를향하는 방향벡터
    _vector3 RestLocalDir = { 0.f, -1.f, 0.f }; 
    _float   fLength = { 0.f }; //그리고 그 방향의 길이

    /* RunTime */
    /* 로컬 모델기준 */
    _vector3 CombinedCurPos{};
    _vector3 CombinedPrevPos{};
    /* 월드 위치기준 */
    _vector3 AnimWorldPos;
    _quaternion AnimWorldQuat{};
    _vector3 DynamicCurPos{};
    _vector3 DynamicPrevPos{};
    _quaternion DynamicCurQuat{};
    _quaternion DynamicPrevQuat{};
}DYNAMIC_NODE;

typedef struct DynamicBoneChainParam
{
    _float Inert        = { 0.0f };             // 0~1 (월드영향)
    _float Damping      = { 0.2f };             // 0~1 (감쇠)
    _float Elasticity   = { 0.002f };           // 0~1 (탄성)
    _float Stiffness    = { 0.1f };             // 0~1 (강성)
    _float GravityScale = { 0.002f };           // 중력 배율
    _vector3 GravityDir = { 0.f, -1.f, 0.f };   // 중력 방향
}CHAIN_PARAM;

typedef struct DynamicBoneChain {
    vector<DYNAMIC_NODE> Nodes;
}DYNAMIC_CHAIN;

typedef struct DynamicBoneChainGroup {
    _int    AnchorBoneIndex = { -1 };
    vector<DYNAMIC_CHAIN> Chains;
    _float UpdateRate = { 60.f };

    _bool       bWorldSpace = { true };    // 월드상 계산을 이용하는지
    CHAIN_PARAM ChainParam{};

    /* RunTime */
    _float UpdateElapsed{};

    _vector3    CurAnchorCombinedPos{};
    _vector3    PrevAnchorCombinedPos{};
    _quaternion CurAnchorCombinedQuat{};
    _quaternion PrevAnchorCombinedQuat{};

    _vector3    CurAnchorWorldPos{};
    _quaternion CurAnchorWorldQuat{};
    _vector3    PrevAnchorWorldPos{};
    _quaternion PrevAnchorWorldQuat{};

}DYNAMIC_CHAIN_GROUP;

NS_END