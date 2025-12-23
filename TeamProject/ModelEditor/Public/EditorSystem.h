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
    unordered_set<string> Get_MappingRules();
   void Push_NamingRules(string MeshName);
   void Push_MappingRules(string MeshName);
   void Pop_NamingRules(string MeshName);
   void Pop_MappingRules(string MeshName);

private:
    unordered_set<string> m_CheckNameRules;

public:
    virtual void Free() override;
};
NS_END