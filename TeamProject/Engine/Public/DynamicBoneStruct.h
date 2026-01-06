#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

typedef struct DynamicBoneNode //모두 컴바인드 TPose 기준
{
    int BoneIndex = -1;
    int ParentIndex = -1;

    // (부모가 원점 -> 부모원점기준 나) 부모로컬로부터 나를향하는 방향벡터
    _vector3 RestLocalDir = { 0.f, -1.f, 0.f }; 
    float fLength = 0.f; //그리고 그 방향의 길이

    _vector3 CombinedCurPos = { 0.f, 0.f, 0.f };
    _vector3 CombinedPrevPos = { 0.f, 0.f, 0.f };
}DYNAMIC_NODE;

typedef struct DynamicBoneChainParam
{
    float fStiffness    = { 0.2f };     // 0~1 (복원력)
    float fDamping      = { 0.1f };     // 0~1 (감쇠)
    float fGravityScale = { 1.0f };     // 중력 배율
}CHAIN_PARAM;

typedef struct DynamicBoneChain {
    vector<DYNAMIC_NODE> Nodes;
}DYNAMIC_CHAIN;

typedef struct DynamicBoneChainGroup {
    string  RootBone;
    _int    RootBoneIndex;
    vector<DYNAMIC_CHAIN> Chains;
    CHAIN_PARAM ChainParam;
}DYNAMIC_CHAIN_GROUP;

NS_END