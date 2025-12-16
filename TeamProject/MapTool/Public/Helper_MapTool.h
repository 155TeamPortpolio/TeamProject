#pragma once
#include "MapTool_Defines.h"

namespace HelperMT {

    // 파일이름_년월일_시분초.확장자명
    inline std::string MakeTimestampFileName(const std::string& prefix, const std::string& ext) 
    {
        const auto now = chrono::system_clock::now();
        const auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;

        const std::time_t nowtime = chrono::system_clock::to_time_t(now);
        // is Not Thread Safe!!        
        std::tm tm = {};
        localtime_s(&tm, &nowtime);

        std::ostringstream oss;
        oss << prefix
            << std::put_time(&tm, "_%Y%m%d_%H%M%S")
            << ext;

        return oss.str(); // prefix_20251216_111530.json
    }

    inline json ToJson_WolrdMatrix(_fmatrix matWorld) {
        _float4x4 World = {};
        XMStoreFloat4x4(&World, matWorld);

        return json{
            { "vRight", {World._11, World._12, World._13, World._14} },
            { "vUp",    {World._21, World._22, World._23, World._24} },
            { "vLook",  {World._31, World._32, World._33, World._34} },
            { "vPos",   {World._41, World._42, World._43, World._44} }
        };
    }
    inline json ToJson_WolrdMatrix(_float4x4* matWorld) {
        return json{
            { "vRight", {matWorld->_11, matWorld->_12, matWorld->_13, matWorld->_14} },
            { "vUp",    {matWorld->_21, matWorld->_22, matWorld->_23, matWorld->_24} },
            { "vLook",  {matWorld->_31, matWorld->_32, matWorld->_33, matWorld->_34} },
            { "vPos",   {matWorld->_41, matWorld->_42, matWorld->_43, matWorld->_44} }
        };
    }

    template <typename T>
    inline void SaveJson_MapTool(T& Data, const string& filePath) {
        nlohmann::ordered_json JsonData = Data;
        ofstream file(filePath);

        if (file.is_open()) {
            file << JsonData.dump(2);
            file.close();
        }
    };
}