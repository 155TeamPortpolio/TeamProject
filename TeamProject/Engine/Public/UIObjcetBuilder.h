#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class ENGINE_DLL CUIObjcetBuilder
{
    public:
        CUIObjcetBuilder(const CLONE_DESC& cloneDesc);
        ~CUIObjcetBuilder();
    public:
        class CUI_Object* Build(const string& instanceKey, _uint* id = nullptr);

    public:
        CUIObjcetBuilder& Offset(const _float2 offset);
        CUIObjcetBuilder& Rotate(const _float rotate);
        CUIObjcetBuilder& Scale(const _float2 scale);
        CUIObjcetBuilder& Size(const _float2 size);
        CUIObjcetBuilder& Anchor(ANCHOR eAnchor);
        CUIObjcetBuilder& Asset(const string& assetKey);

    private:
        class CGameInstance* m_pGameInstance = { nullptr };
        CLONE_DESC* m_CloneDesc = { nullptr };
        UI_DESC* m_pObjDesc = { nullptr };
        unordered_map<type_index, COMPONENT_DESC*> m_CompDesc;
};

NS_END