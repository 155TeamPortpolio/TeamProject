#pragma once
#include "Engine_Defines.h"
#include "MapData_Defines.h"
#include <limits>


namespace Client {

#pragma region Helper/Load SlotData From Json  
    inline SLOT_DATA_TYPE ParseType(const std::string& tagType)
    {
        if (tagType == "Int")    return SLOT_DATA_TYPE::Int;
        if (tagType == "Float")  return SLOT_DATA_TYPE::Float;
        if (tagType == "Bool")   return SLOT_DATA_TYPE::Bool;
        if (tagType == "String") return SLOT_DATA_TYPE::String;
        if (tagType == "Float3") return SLOT_DATA_TYPE::Float3;
        return SLOT_DATA_TYPE::END;
    }


    template <class T>
    inline bool TryGet(const json& obj, const _char* key, T& out)
    {
        if (!obj.is_object() || !obj.contains(key))
            return false;

        try {
            out = obj.at(key).get<T>(); // contains 후 at()은 안전
            return true;
        }
        catch (...) {
            return false; // 타입 불일치 등은 무시
        }
    }

    inline bool TryGetInt(const json& obj, const _char* key, _int& Out)
    {
        return TryGet<_int>(obj, key, Out);
    }
    inline bool TryGetFloat(const json& obj, const _char* key, _float& Out)
    {
        return TryGet<_float>(obj, key, Out);
    }
    inline bool TryGetBool(const json& obj, const _char* key, _bool& Out)
    {
        return TryGet<_bool>(obj, key, Out);
    }
    inline bool TryGetString(const json& obj, const _char* key, string& Out)
    {
        return TryGet<string>(obj, key, Out);
    }

    inline const json* FindPtr(const json& obj, const _char* key)
    {
        if (!obj.is_object()) 
            return nullptr;
        auto it = obj.find(key);
        return (it == obj.end()) ? nullptr : &(*it);
    }

    inline bool TryReadString(const json& obj, const _char* key, string& out)
    {
        const json* p = FindPtr(obj, key);
        if (!p || !p->is_string()) 
            return false;
        out = p->get<std::string>();
        return true;
    }

    inline bool TryReadInt(const json& obj, const char* key, _int& out)
    {
        const json* p = FindPtr(obj, key);
        if (!p) return false;

        if (p->is_number_integer() || p->is_number_unsigned()) {
            const int64_t v = p->get<int64_t>(); 
            if (v < (std::numeric_limits<_int>::min)() ||
                v > (std::numeric_limits<_int>::max)())
                return false;
            out = static_cast<_int>(v);
            return true;
        }
        return false;
    }

    inline _bool TryParseFloat3(const json& j, _float3& out)
    {
        if (j.is_array() && j.size() >= 3)
        {
            try {
                out = _float3{
                    j.at(0).get<float>(),
                    j.at(1).get<float>(),
                    j.at(2).get<float>()
                };
                return true;
            }
            catch (...) {
                return false;
            }
        }

        if (j.is_object())
        {
            _float x, y, z;
            if (TryGetFloat(j, "x", x) && TryGetFloat(j, "y", y) && TryGetFloat(j, "z", z))
            {
                out = _float3{ x, y, z };
                return true;
            }
        }

        return false;
    }
    
    inline bool TryParseSlotValue(const json& obj, SlotValue& out)
    {
        out = {}; 

        if (!obj.is_object())
            return false;

        std::string typeStr;
        if (!TryGetString(obj, "type", typeStr))
            return false;

        out.type = ParseType(typeStr);

        // value가 없으면 구조가 없다 판단
        if (!obj.contains("value"))
            return false;

        const json& v = obj.at("value");

        try {
            switch (out.type)
            {
            case SLOT_DATA_TYPE::Int:
                out.value = v.get<_int>();
                return true;

            case SLOT_DATA_TYPE::Float:
                out.value = v.get<_float>();
                return true;

            case SLOT_DATA_TYPE::Bool:
                out.value = v.get<_bool>();
                return true;

            case SLOT_DATA_TYPE::String:
                out.value = v.get<std::string>();
                return true;

            case SLOT_DATA_TYPE::Float3: {
                XMFLOAT3 f3{};
                if (!TryParseFloat3(v, f3))
                    return false;
                out.value = f3;
                return true;
            }

            default:
                return false;
            }
        }
        catch (...) {
            // 타입 불일치(type_error 등)는 무시
            return false;
        }
    }

    static bool TryParseFieldData(const json& elem, FIELD_DATA& out)
    {
        out = {};
        if (!elem.is_object())
            return false;

        // iObjID는 옵션: 없으면 -1 유지
        int32_t objId = -1;
        if (TryGetInt(elem, "iObjID", objId))
            out.iObjID = objId;

        // TagName은 필수로 보고, 없거나 비어있으면 “무시”
        if (!TryGetString(elem, "TagName", out.TagName) || out.TagName.empty())
            return false;

        // defaultvalue는 필수로 보고, 구조가 없거나 타입/값이 이상하면 “무시”
        if (!elem.contains("defaultvalue"))
            return false;

        const json& dv = elem.at("defaultvalue");
        if (!TryParseSlotValue(dv, out.defaultvalue))
            return false;

        return true;
    }
#pragma endregion

#pragma region Helper/Use SlotValue Data
    template<typename T> struct SlotTypeTag;

    template<> struct SlotTypeTag<int32_t> { static constexpr SLOT_DATA_TYPE tag = SLOT_DATA_TYPE::Int; };
    template<> struct SlotTypeTag<float> { static constexpr SLOT_DATA_TYPE tag = SLOT_DATA_TYPE::Float; };
    template<> struct SlotTypeTag<bool> { static constexpr SLOT_DATA_TYPE tag = SLOT_DATA_TYPE::Bool; };
    template<> struct SlotTypeTag<std::string> { static constexpr SLOT_DATA_TYPE tag = SLOT_DATA_TYPE::String; };
    template<> struct SlotTypeTag<DirectX::XMFLOAT3> { static constexpr SLOT_DATA_TYPE tag = SLOT_DATA_TYPE::Float3; };

    template<typename T>
    optional<T> GetSlotValue(const SlotValue& sv)
    {
        // SlotValue의 값과 완전히 일치하는 값만 가능
        static_assert(
            std::is_same_v<T, _int> ||
            std::is_same_v<T, _float> ||
            std::is_same_v<T, _bool> ||
            std::is_same_v<T, std::string> ||
            std::is_same_v<T, _float3>,
            "Func<T>: unsupported type"
            );

        if (sv.type != SlotTypeTag<T>::tag)
            return nullopt;

        if (auto p = std::get_if<T>(&sv.value)) // 타입 아니면 nullptr
            return *p;

        return nullopt; // type과 variant가 어긋난 경우도 안전하게 실패
    }



#pragma endregion

}