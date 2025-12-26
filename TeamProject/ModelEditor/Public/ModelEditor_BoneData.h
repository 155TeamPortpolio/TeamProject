#pragma once
#include "ModelEditor_Defines.h"
#include "Json_Inc/json.hpp"

namespace DirectX {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(XMFLOAT4X4,
        _11, _12, _13, _14,
        _21, _22, _23, _24,
        _31, _32, _33, _34,
        _41, _42, _43, _44
    );
}

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
        string      TagModel = {};
        vector<BONE_INFO>   BoneInfos;
    }BONE_DATA_HEADER;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BONE_DATA_HEADER, TagModel, BoneInfos);

}