#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)NS_BEGIN(Engine)

enum class HumanoidBone : _uint
{
    Root,

    Pelvis,
    Spine1, Spine2, Chest,
    Neck, Head,

    ClavicleL, UpperArmL, LowerArmL, HandL,
    ClavicleR, UpperArmR, LowerArmR, HandR,

    ThighL, CalfL, FootL, ToeL,
    ThighR, CalfR, FootR, ToeR,

    Count
};

struct HumanoidMap
{
    _int index[ENUM(HumanoidBone::Count)];
};

inline void InitHumanoidMap(HumanoidMap& mapRef)
{
    for (_uint i = 0; i < ENUM(HumanoidBone::Count); ++i)
        mapRef.index[i] = -1;
}

inline _bool HasBone(const HumanoidMap& mapRef, HumanoidBone role)
{
    return mapRef.index[ENUM(role)] >= 0;
}

struct LegChainIndex
{
    _int pelvisIndex;
    _int thighIndex;
    _int calfIndex;
    _int footIndex;
    _int toeIndex;
};

NS_END




