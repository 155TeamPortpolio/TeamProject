#pragma once
#include "Component.h"
NS_BEGIN(Engine)

class ENGINE_DLL CObjectContainer  final:
    public CComponent
{
private:
    CObjectContainer();
    CObjectContainer(const CObjectContainer& rhs);
    virtual ~CObjectContainer() override;

public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(COMPONENT_DESC* pArg) override;
    virtual void Pre_EngineUpdateChild(_float dt);
    virtual void Post_EngineUpdateChild(_float dt);
    void Priority_UpdateChild(_float dt) ;
    void UpdateChild(_float dt) ;
    void Late_UpdateChild(_float dt) ;

public:
    class CGameObject* Find_ObjectByName(const string& ObjectName);
    class CGameObject* Find_ObjectByID(_uint ObjectID);
    _int Find_IndexByName(const string& ObjectName);
    _int Find_IndexByID(_uint ObjectID);

    _int Add_Child(class CGameObject* pObject, _bool SyncTransform = true);
    void Destroy_Child(_uint ChildIndex);
    void Dettach_Child(_uint ChildIndex);

public:
    void Render_GUI() override;

public:
    void ReorderChildren(class CGameObject* pObject, _uint Index);
    void Upper_Order_Children(class CGameObject* pObject);
    void Lower_Order_Children(class CGameObject* pObject);
    void Set_Order_Top(class CGameObject* pObject);
    void Set_Order_Bottom(class CGameObject* pObject);
public:
    const vector<class CGameObject*> Get_Children() { return m_ChildrenObjects; };

private:    
    map<_uint, string> m_ChildrensName; /*오브젝트  ID, 이름*/
    map<_uint, _uint> m_IndexByID; /*오브젝트  ID, 인덱스*/
    vector<class CGameObject*> m_ChildrenObjects;

    vector<_uint> m_UpdateOrder;                 
    unordered_map<_uint, _uint> m_OrderIndexByID; 

public:
    static CObjectContainer* Create();
    virtual CComponent* Clone();
    virtual void Free() override;
};

NS_END