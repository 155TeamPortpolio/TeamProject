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
        CUIObjcetBuilder& Add_To_Level(const string& Level);/*{level,layer}*/
    public:
        CUIObjcetBuilder& Position(const _float2 position);
        CUIObjcetBuilder& Rotate(const _float rotate);
        CUIObjcetBuilder& Scale(const _float2 scale);
        CUIObjcetBuilder& Set_Anchor(ANCHOR eAnchor, _float2 vPivot);

    private:
        class CGameInstance* m_pGameInstance = { nullptr };
        string m_LevelTag = {};
        CLONE_DESC* m_CloneDesc = { nullptr };
        UI_DESC* m_pObjDesc = { nullptr };

        _bool m_bPivoted = { false };
        ANCHOR m_eAnchor = { ANCHOR::Center};
        _float2 m_vPivot = {};
        unordered_map<type_index, COMPONENT_DESC*> m_CompDesc;
};

NS_END