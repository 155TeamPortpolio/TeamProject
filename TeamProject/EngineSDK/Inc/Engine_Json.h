#pragma once
#include "Engine_Defines.h"
#include "Json_Inc/json.hpp"

namespace DirectX {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(XMFLOAT4X4,
        _11, _12, _13, _14,
        _21, _22, _23, _24,
        _31, _32, _33, _34,
        _41, _42, _43, _44
    );
    
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(XMFLOAT2,
        x, y
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(XMFLOAT3,
        x, y, z
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(XMFLOAT4,
        x, y, z, w
    );
}