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
        CUIObjcetBuilder& Add_UIDesc(UI_DESC* pArg);
        CUIObjcetBuilder& WorldPos(const _float3 worldPos);
        CUIObjcetBuilder& FromPool();

    private:
        class CGameInstance* m_pGameInstance = { nullptr };
        CLONE_DESC m_CloneDesc = {  };
        UI_DESC* m_pObjDesc = { nullptr };
        unordered_map<type_index, COMPONENT_DESC*> m_CompDesc;
        _bool m_isFromPool = { false };
};

NS_END