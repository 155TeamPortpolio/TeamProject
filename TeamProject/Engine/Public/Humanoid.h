#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)NS_BEGIN(Engine)

#pragma once
#include <array>
#include <string>
#include <cstdint>

NS_BEGIN(Engine)

// 1) 휴머노이드 "역할" 정의 (툴/런타임 공용)
enum class HumanoidBone : uint8_t
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

// 2) 본 인덱스 매핑 (-1이면 없음)
struct HumanoidMap
{
    array<_int, HUMANOID_BONE_COUNT> indexByRole;

    void Reset()
    {
        indexByRole.fill(-1);
    }

    int32_t Get(HumanoidBone role) const
    {
        return indexByRole[static_cast<uint32_t>(role)];
    }

    void Set(HumanoidBone role, int32_t boneIndex)
    {
        indexByRole[static_cast<uint32_t>(role)] = boneIndex;
    }

    bool Has(HumanoidBone role) const
    {
        return Get(role) >= 0;
    }
};

// 3) 체인 구조들 (툴에서 UI/검증/저장/프리뷰에 바로 쓰기 좋게)
struct TwoBoneChain
{
    _int upperIndex = -1;   // UpperArm / Thigh
    _int lowerIndex = -1;   // LowerArm / Calf
    _int endIndex = -1;   // Hand / Foot
    _int tipIndex = -1;   // (옵션) Toe / (손가락 끝 등)

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

// 4) 툴용 휴머노이드 데이터(= 편집 대상, 저장 대상)
struct HumanoidRigData
{
    // 식별(툴에서 여러 스켈레톤 열었을 때 구분용)
   string skeletonName;
    uint64_t skeletonGuid = 0; // 옵션: 해시/UUID 등

    HumanoidMap map;

    // 미리 만들어 둔 “역할 기반 체인”
    SpineChain spine;
    TwoBoneChain leftLeg;
    TwoBoneChain rightLeg;
    TwoBoneChain leftArm;
    TwoBoneChain rightArm;

    // 옵션: 손/발 IK 타겟(모델에 실제로 존재하면 매핑해둘 수 있음)
    // IK 타겟 본을 스켈레톤에 강제하진 않되, 있으면 툴에서 관리하기 편함.
    _int leftFootIKTargetIndex = -1;
    _int rightFootIKTargetIndex = -1;
    _int leftHandIKTargetIndex = -1;
    _int rightHandIKTargetIndex = -1;

    // 옵션: 무기 소켓(본 인덱스 + 로컬 오프셋으로 관리하는 걸 추천)
    _int weaponSocketParentIndex = -1; // 보통 HandR
    // 툴에서 오프셋은 Transform(로컬)로 따로 들고 있으면 됨(여긴 인덱스만)

    void Reset()
    {
        skeletonName.clear();
        skeletonGuid = 0;
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

    // Map 기반으로 체인을 자동 재구성(툴에서 매핑 수정할 때마다 호출)
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

        // 무기 소켓 기본값: HandR
        weaponSocketParentIndex = map.Get(HumanoidBone::HandR);
    }

    // 최소 유효성(툴 경고용)
    bool IsMinimallyUsableForFootIK() const
    {
        const bool hasPelvis = spine.pelvisIndex >= 0;
        return hasPelvis && leftLeg.IsValid() && rightLeg.IsValid();
    }

    bool IsMinimallyUsableForHandIK() const
    {
        return leftArm.IsValid() && rightArm.IsValid();
    }
};

NS_END

NS_END




