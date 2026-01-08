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
    _vector3 DynamicCurPos{};
    _vector3 DynamicPrevPos{};
}DYNAMIC_NODE;

typedef struct DynamicBoneChainParam
{
    _float fStiffness    = { 0.1f };     // 0~1 (강성)
    _float fDamping      = { 0.1f };     // 0~1 (점성)
    _float fElasticity   = { 0.1f };     // 
    _float fInert        = { 0.1f };     // 0~1 (감쇠)
    _float fGravityScale = { 1.0f };     // 중력 배율
}CHAIN_PARAM;

typedef struct DynamicBoneChain {
    vector<DYNAMIC_NODE> Nodes;
}DYNAMIC_CHAIN;

typedef struct DynamicBoneChainGroup {
    _int    AnchorBoneIndex = { -1 };
    vector<DYNAMIC_CHAIN> Chains;

    _bool       bWorldSpace = { true };    // 월드상 계산을 이용하는지
    CHAIN_PARAM ChainParam{};

    /* RunTime */
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