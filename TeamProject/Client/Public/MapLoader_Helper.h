#pragma once
#include "Engine_Defines.h"
#include "MapData_Defines.h"

namespace Client {

    inline SLOT_DATA_TYPE ParseType(const std::string& s)
    {
        if (s == "Int")    return SLOT_DATA_TYPE::Int;
        if (s == "Float")  return SLOT_DATA_TYPE::Float;
        if (s == "Bool")   return SLOT_DATA_TYPE::Bool;
        if (s == "String") return SLOT_DATA_TYPE::String;
        if (s == "Float3") return SLOT_DATA_TYPE::Float3;
        return SLOT_DATA_TYPE::NONE;
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

    inline bool TryGetInt(const json& obj, const _char* key, int32_t& Out)
    {
        return TryGet<int32_t>(obj, key, Out);
    }
    inline bool TryGetFloat(const json& obj, const _char* key, double& Out)
    {
        return TryGet<double>(obj, key, Out);
    }
    inline bool TryGetBool(const json& obj, const _char* key, bool& Out)
    {
        return TryGet<bool>(obj, key, Out);
    }
    inline bool TryGetString(const json& obj, const _char* key, string& Out)
    {
        return TryGet<string>(obj, key, Out);
    }
    inline bool TryGetFloat3(const json& obj, const _char* key, XMFLOAT3& Out)
    {
        return TryGet<XMFLOAT3>(obj, key, Out);
    }

    inline _bool TryParseFloat3(const json& j, XMFLOAT3& out)
    {
        if (j.is_array() && j.size() >= 3)
        {
            try {
                out = XMFLOAT3{
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
            double x, y, z;
            if (TryGetFloat(j, "x", x) && TryGetFloat(j, "y", y) && TryGetFloat(j, "z", z))
            {
                out = XMFLOAT3{ x, y, z };
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
                out.value = v.get<int>();
                return true;

            case SLOT_DATA_TYPE::Float:
                out.value = v.get<float>();
                return true;

            case SLOT_DATA_TYPE::Bool:
                out.value = v.get<bool>();
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
        int objId = -1;
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
}