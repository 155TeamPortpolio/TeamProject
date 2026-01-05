#pragma once
#include "Engine_Defines.h"

struct SlotNameRule
{
    HumanoidBone slot;
    const char* keywords[4]; // 간단히 3~4개면 충분
};

static const SlotNameRule s_AutoMapRules[] =
{
    { HumanoidBone::Pelvis,    { "bip001 pelvis", nullptr, nullptr } },
    { HumanoidBone::Spine,    { "bip001 spine", nullptr, nullptr } },
    { HumanoidBone::Spine1,    { "bip001 spine1", nullptr, nullptr } },
    { HumanoidBone::Spine2,    { "bip001 spine2", nullptr, nullptr } },
    { HumanoidBone::Chest,     { "bip001 chest", nullptr, nullptr } },
    { HumanoidBone::Neck,      { "bip001 neck", nullptr } },
    { HumanoidBone::Head,      { "bip001 head", nullptr } },

    { HumanoidBone::ClavicleL,      { "bip001 l clavicle", nullptr } },
    { HumanoidBone::UpperArmL, { "bip001 l upperarm", nullptr, nullptr, nullptr } },
    { HumanoidBone::LowerArmL, { "bip001 l forearm", nullptr, nullptr } },
    { HumanoidBone::HandL,     { "bip001 l hand", nullptr, nullptr } },

    { HumanoidBone::ClavicleR,      { "bip001 r clavicle", nullptr } },
    { HumanoidBone::UpperArmR, { "bip001 r upperarm", nullptr, nullptr, nullptr } },
    { HumanoidBone::LowerArmR, { "bip001 r forearm", nullptr, nullptr } },
    { HumanoidBone::HandR,     { "bip001 r hand", nullptr, nullptr } },

    { HumanoidBone::ThighL,    { "bip001 l thigh", nullptr, nullptr } },
    { HumanoidBone::CalfL,     { "bip001 l calf", nullptr, nullptr } },
    { HumanoidBone::FootL,     { "bip001 l foot", nullptr, nullptr } },
    { HumanoidBone::ToeL,     { "bip001 l toe", nullptr, nullptr } },

    { HumanoidBone::ThighR,    { "bip001 r thigh", nullptr, nullptr } },
    { HumanoidBone::CalfR,     { "bip001 r calf", nullptr, nullptr } },
    { HumanoidBone::FootR,     { "bip001 r foot", nullptr, nullptr } },
    { HumanoidBone::ToeR,     { "bip001 r toe", nullptr, nullptr } },
};
