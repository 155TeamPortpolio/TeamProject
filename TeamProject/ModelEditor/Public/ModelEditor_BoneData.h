#pragma once
#include "ModelEditor_Defines.h"
#include "Json_Inc/json.hpp"

namespace ModelEdit {

    typedef struct tagBoneInfo {
        _int        Index = { -1 } ;
        string      TagBone = {};
        XMFLOAT4X4   BoneOffsetMatrix = {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f };
    }BONE_INFO;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BONE_INFO, Index, TagBone, BoneOffsetMatrix);

    typedef struct tagBoneDataHeader {
        string		TagDataFormat = {};
        string      TagModel = {};
        vector<BONE_INFO>   BoneInfos;
    }BONE_DATA_HEADER;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BONE_DATA_HEADER, TagDataFormat, TagModel, BoneInfos);

}