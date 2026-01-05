#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)


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

constexpr _uint HUMANOID_BONE_COUNT = static_cast<_uint>(HumanoidBone::Count);

struct HumanoidMap
{
    array<_int, HUMANOID_BONE_COUNT> indexByRole;

    void Reset()
    {
        indexByRole.fill(-1);
    }

    _uint Get(HumanoidBone role) const { return indexByRole[static_cast<_uint>(role)]; }
    void Set(HumanoidBone role, _uint boneIndex) { indexByRole[static_cast<uint32_t>(role)] = boneIndex; }
    bool Has(HumanoidBone role) const {return Get(role) >= 0; }
};

struct TwoBoneChain
{
    _int upperIndex = -1;    // UpperArm / Thigh
    _int lowerIndex = -1;     // LowerArm / Calf
    _int endIndex = -1;        // Hand / Foot
    _int tipIndex = -1;          // (옵션) Toe / (손가락 끝 등)

    bool IsValid() const
    {
        return upperIndex >= 0 && lowerIndex >= 0 && endIndex >= 0;
    }
};

struct SpineChain
{
    _int pelvisIndex = -1;
    _int spine1Index = -1;
    _int spine2Index = -1;
    _int chestIndex = -1;
    _int neckIndex = -1;
    _int headIndex = -1;

    bool HasPelvis() const { return pelvisIndex >= 0; }
    bool HasHead()   const { return headIndex >= 0; }
};

struct HumanoidRigData
{
   string skeletonName;
    HumanoidMap map;

    //역할 체인
    SpineChain spine;
    TwoBoneChain leftLeg;
    TwoBoneChain rightLeg;
    TwoBoneChain leftArm;
    TwoBoneChain rightArm;

    // 손/발 IK 타겟
    _int leftFootIKTargetIndex = -1;
    _int rightFootIKTargetIndex = -1;
    _int leftHandIKTargetIndex = -1;
    _int rightHandIKTargetIndex = -1;

    //무기 소켓
    _int weaponSocketParentIndex = -1; 

    void Reset()
    {
        skeletonName.clear();
        map.Reset();

        spine = SpineChain{};
        leftLeg = TwoBoneChain{};
        rightLeg = TwoBoneChain{};
        leftArm = TwoBoneChain{};
        rightArm = TwoBoneChain{};

        leftFootIKTargetIndex = -1;
        rightFootIKTargetIndex = -1;
        leftHandIKTargetIndex = -1;
        rightHandIKTargetIndex = -1;

        weaponSocketParentIndex = -1;
    }

    void RebuildChainsFromMap()
    {
        spine.pelvisIndex = map.Get(HumanoidBone::Pelvis);
        spine.spine1Index = map.Get(HumanoidBone::Spine1);
        spine.spine2Index = map.Get(HumanoidBone::Spine2);
        spine.chestIndex = map.Get(HumanoidBone::Chest);
        spine.neckIndex = map.Get(HumanoidBone::Neck);
        spine.headIndex = map.Get(HumanoidBone::Head);

        leftLeg.upperIndex = map.Get(HumanoidBone::ThighL);
        leftLeg.lowerIndex = map.Get(HumanoidBone::CalfL);
        leftLeg.endIndex = map.Get(HumanoidBone::FootL);
        leftLeg.tipIndex = map.Get(HumanoidBone::ToeL);

        rightLeg.upperIndex = map.Get(HumanoidBone::ThighR);
        rightLeg.lowerIndex = map.Get(HumanoidBone::CalfR);
        rightLeg.endIndex = map.Get(HumanoidBone::FootR);
        rightLeg.tipIndex = map.Get(HumanoidBone::ToeR);

        leftArm.upperIndex = map.Get(HumanoidBone::UpperArmL);
        leftArm.lowerIndex = map.Get(HumanoidBone::LowerArmL);
        leftArm.endIndex = map.Get(HumanoidBone::HandL);
        leftArm.tipIndex = -1;

        rightArm.upperIndex = map.Get(HumanoidBone::UpperArmR);
        rightArm.lowerIndex = map.Get(HumanoidBone::LowerArmR);
        rightArm.endIndex = map.Get(HumanoidBone::HandR);
        rightArm.tipIndex = -1;

        weaponSocketParentIndex = map.Get(HumanoidBone::HandR);
    }
};

NS_END



