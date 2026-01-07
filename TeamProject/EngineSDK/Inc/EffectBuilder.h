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
    CEffectBuilder& Asset(const string& assetKey);
    CEffectBuilder& Position(const _float3 position);
    //CEffectBuilder& Rotate(const _float rotate);
    CEffectBuilder& Scale(const _float3 scale);
    CEffectBuilder& FromPool() { m_isFromPool = true; return *this; };

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    CLONE_DESC m_CloneDesc = {  };
    EFFECT_DESC* m_pObjDesc = { nullptr };
    unordered_map<type_index, COMPONENT_DESC*> m_CompDesc;
    _bool m_isFromPool = { false };
};
NS_END
