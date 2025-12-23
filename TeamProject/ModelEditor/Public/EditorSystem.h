#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CLayer;
class CMode;
class CMaterial;
NS_END


NS_BEGIN(ModelEdit)
class CEditorSystem :
    public CBase
{
    struct SelectedMesh
    {
        _float4x4* pWorldMatrix = { nullptr };
        CModel* pModel = { nullptr };
        CMaterial* pMaterial = { nullptr };
    };

    DECLARE_SINGLETON(CEditorSystem);
private:
    explicit CEditorSystem();
    virtual ~CEditorSystem() DEFAULT;

public:
    HRESULT Initialize();
    void Update();

public:
    _bool CheckNamingRule(string MeshName);
    unordered_set<string> Get_NamingRules();
   void Push_NamingRules(string MeshName);
   void Pop_NamingRules(string MeshName);

    unordered_set<string> Get_MappingRules();
   void Push_MappingRules(string MeshName);
   void Pop_MappingRules(string MeshName);

private:
    class CRayCaster* m_pRayCast = { nullptr };
    CLayer* m_pModelLayer = { nullptr };
    SelectedMesh mesh = {};

    vector<string> m_Directorys;
    unordered_map<string, MaterialTexNames> m_matTexNames;

public:
    virtual void Free() override;
};
NS_END