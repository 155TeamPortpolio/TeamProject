#pragma once
#include "Engine_Defines.h"
#include "Helper_Func.h"

NS_BEGIN(Engine)

typedef struct DynamicBoneNode 
{
    _int BoneIndex = { -1 };
    _int ParentIndex = { -1 };

    // (부모가 원점 -> 부모원점기준 나) 부모로컬로부터 나를향하는 방향벡터
    _vector3 RestLocalDir = { 0.f, -1.f, 0.f }; 
    _float   fLength = { 0.f }; //그리고 그 방향의 길이

    /* RunTime */
    Vector3 TipPrevCombinedPos;
    Vector3 TipCurCombinedPos;
}DYNAMIC_NODE;

typedef struct DistributionParam
{
    EaseType Ease = EaseType::Linear;
    float    Start = 1.f;
    float    End = 0.f;
}DISTRIB;

typedef struct DynamicBoneChainParam
{
    _float  Inert        = { 0.0f };           // 0~1 (월드영향)
    DISTRIB Inert_Distrib{};
    _float  Damping      = { 0.0f };           // 0~1 (감쇠)
    DISTRIB Damping_Distrib{};
    _float  Elasticity   = { 0.0f };           // 0~1 (탄성)
    DISTRIB Elasticity_Distrib{};
    _float  Stiffness    = { 0.0f };           // 0~1 (강성)
    DISTRIB Stiffness_Distrib{};
    _float GravityScale = { 0.0f };           // 중력 배율
    _vector3 GravityDir = { 0.f, -1.f, 0.f }; // 중력 방향
}CHAIN_PARAM;

typedef struct DynamicBoneChain {
    vector<DYNAMIC_NODE> Nodes;
}DYNAMIC_CHAIN;

typedef struct DynamicBoneChainGroup {
    _int    AnchorBoneIndex = { -1 };
    vector<DYNAMIC_CHAIN> Chains;
    _float UpdateRate = { 60.f };
    CHAIN_PARAM ChainParam{};

    /* RunTime */
    _float UpdateElapsed{};

    _vector3    CurAnchorCombinedPos{};
    _vector3    PrevAnchorCombinedPos{};
}DYNAMIC_CHAIN_GROUP;

NS_END