#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

typedef struct DynamicNode //모두 컴바인드 TPose 기준
{
    int BoneIndex = -1;
    int ParentIndex = -1;

    float fLength = 0.f;                        // 부모뼈로부터 컴바인드 길이
    _vector3 RestLocalDir = { 0.f, -1.f, 0.f }; // (부모 -> 나) 의방향벡터

    _vector3 WorldCurPos = { 0.f, 0.f, 0.f };
    _vector3 WorldPrevPos = { 0.f, 0.f, 0.f };
}DYNAMIC_NODE;

typedef struct DynamicChainParam
{
    float fStiffness    = { 0.2f };     // 0~1 (복원력)
    float fDamping      = { 0.1f };     // 0~1 (감쇠)
    float fGravityScale = { 1.0f };     // 중력 배율
}CHAIN_PARAM;

typedef struct DynamicChain {
    string  RootBone;
    _int    RootBoneIndex;
    vector<DynamicNode> Chains;
    CHAIN_PARAM ChainParam;
}DYNAMIC_CHAIN;

NS_END