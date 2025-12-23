#pragma once
#include "Base.h"



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
   void Push_NamingRules(string MeshName);
   void Pop_NamingRules(string MeshName);

private:
    unordered_set<string> m_CheckNameRules;

public:
    virtual void Free() override;
};
NS_END