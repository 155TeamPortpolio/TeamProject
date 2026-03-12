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

public:
    _bool Get_Island() { return m_islandParse;}
    void Set_Island(_bool island) { m_islandParse = island; }
private:
    unordered_set<string> m_CheckNameRules;
    _bool m_islandParse = {};

public:
    virtual void Free() override;
};
NS_END