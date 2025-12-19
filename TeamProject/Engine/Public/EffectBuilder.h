#pragma once

NS_BEGIN(Engine)
class ENGINE_DLL CEffectBuilder
{
public:
    CEffectBuilder(const CLONE_DESC& cloneDesc);
    ~CEffectBuilder();
public:
    class CEffectContainer* Build(const string& instanceKey, _uint* id = nullptr);

public:
    CEffectBuilder& Position(const _float3 position);
    CEffectBuilder& Rotate(const _float rotate);
    CEffectBuilder& Scale(const _float2 scale);

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    CLONE_DESC* m_CloneDesc = { nullptr };
    UI_DESC* m_pObjDesc = { nullptr };
    unordered_map<type_index, COMPONENT_DESC*> m_CompDesc;
};
NS_END
