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
    
    // string 경로 넣어도 됨
    inline bool EnsureDirectoryExists(const filesystem::path& dir)
    {
        filesystem::path directory = dir;

        error_code ec;

        if (filesystem::exists(directory, ec))
            return filesystem::is_directory(directory, ec); // 폴더면 true, 파일이면 false

        filesystem::create_directories(directory, ec);      // 중간 경로까지 생성
        return !ec && filesystem::is_directory(directory, ec);
    }

    inline _bool ExtractSRT(const _matrix& mat, _float3& OutScale, _float4& OutRotQ, _float3& OutTrans) 
    {
        XMVECTOR S, R, T;

        if (!XMMatrixDecompose(&S, &R, &T, mat))
            return false;

        XMStoreFloat3(&OutScale, S);
        XMStoreFloat4(&OutRotQ, R);
        XMStoreFloat3(&OutTrans, T);
        return true;
    }

    inline _float3 QuaternionToEuler(const _float4& q) 
    {
        const _float x = q.x, y = q.y, z = q.z, w = q.w;

        // pitch (x-axis)
        _float sinp = 2.0f * (w * x - y * z);
        _float pitch;
        if (fabsf(sinp) >= 1.0f)
            pitch = copysignf(XM_PIDIV2, sinp);
        else
            pitch = asinf(sinp);

        // yaw (y-axis)
        _float yaw = atan2f(2.0f * (w * y + z * x),
            1.0f - 2.0f * (x * x + y * y));

        // roll (z-axis)
        _float roll = atan2f(2.0f * (w * z + x * y),
            1.0f - 2.0f * (z * z + x * x));

        return XMFLOAT3(pitch, yaw, roll);
    }

    template <typename T>
    inline bool ExportJsonFile(T& Data, const string& filePath)
    {
        nlohmann::ordered_json JsonData = Data;

        filesystem::path p(filePath);
        filesystem::path dir = p.parent_path();

        std::error_code ec;

        if (!dir.empty())
        {
            if (!filesystem::exists(dir, ec) && !ec)
                filesystem::create_directories(dir, ec);

            if (ec) 
                return false;
        }

        std::ofstream ofs(p, std::ios::out | std::ios::trunc); 
        if (!ofs) 
            return false;

        ofs << JsonData.dump(2);
        if (!ofs) 
            return false;

        ofs.close();
        return !ofs.fail();
    }
} 

namespace nlohmann
{
    template<>
    struct adl_serializer<DirectX::XMFLOAT3>
    {
        template <class BasicJsonType>
        static void to_json(BasicJsonType& j, const DirectX::XMFLOAT3& v)
        {
            j = BasicJsonType::array({ v.x, v.y, v.z });
        }

        template <class BasicJsonType>
        static void from_json(const BasicJsonType& j, DirectX::XMFLOAT3& v)
        {
            v.x = j.at(0).template get<float>();
            v.y = j.at(1).template get<float>();
            v.z = j.at(2).template get<float>();
        }
    };


    const inline _char* SlotTypeName(const SlotValue& value)
    {
        return std::visit([](auto&& arg) -> const _char*
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>)             return "Int";
                if constexpr (std::is_same_v<T, double>)              return "Float";
                if constexpr (std::is_same_v<T, bool>)                return "Bool";
                if constexpr (std::is_same_v<T, std::string>)         return "String";
                if constexpr (std::is_same_v<T, DirectX::XMFLOAT2>)   return "Float2";
                if constexpr (std::is_same_v<T, DirectX::XMFLOAT3>)   return "Float3";
                if constexpr (std::is_same_v<T, DirectX::XMFLOAT4>)   return "Float4";
                return "END";
            }, value);
    }

    template<>
    struct adl_serializer<SlotValue>
    {
        template <class BasicJsonType>
        static void to_json(BasicJsonType& j, const SlotValue& value)
        {
            j = BasicJsonType::object();
            j["type"] = SlotTypeName(value);

            std::visit([&](auto&& arg)
                {
                    j["value"] = arg; // XMFLOAT3는 위 adl_serializer로 자동 처리됨
                }, value);
        }

        template <class BasicJsonType>
        static void from_json(const BasicJsonType& j, SlotValue& value)
        {
            const std::string t = j.at("type").template get<std::string>();

            if (t == "Int")    value = j.at("value").template get<int64_t>();
            else if (t == "Float")  value = j.at("value").template get<double>();
            else if (t == "Bool")   value = j.at("value").template get<bool>();
            else if (t == "String") value = j.at("value").template get<std::string>();
            else if (t == "Float2") value = j.at("value").template get<DirectX::XMFLOAT2>();
            else if (t == "Float3") value = j.at("value").template get<DirectX::XMFLOAT3>();
            else if (t == "Float4") value = j.at("value").template get<DirectX::XMFLOAT4>();
            else                    value = int64_t{ 0 };
        }
    };

    
}