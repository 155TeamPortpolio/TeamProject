#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CCollider abstract:
    public CComponent
{
protected:
    CCollider();
    CCollider(const CCollider& rhs);
    virtual ~CCollider() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;
    virtual void Update() PURE;
    virtual void Late_Update() PURE;

public:
   virtual _bool Intersect(COLLIDER_SLOT* pSlot) PURE;
   virtual _bool Has_Desc() PURE;
   virtual void Make_MinMaxCollider(MINMAX_BOX minMax)PURE;
   virtual void Set_ColliderActive(_bool Active);

#ifdef _DEBUG
    virtual void Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) PURE;
#endif

public:
    virtual COLLIDER_TYPE Get_ColliderType() PURE;
    COLLISION_CONTEXT Get_Context() { return m_CollisionContext; };
    void Set_Context(class CGameObject* pObject,const string& eventTag) { m_CollisionContext.Owner = pObject; m_CollisionContext.EventTag = eventTag;};
    void Set_ContextOwner(class CGameObject* pObject) { m_CollisionContext.Owner = pObject; };
    void Set_ContextEvent(const string& eventTag) { m_CollisionContext.EventTag = eventTag; };
    void Reset_Context() { m_CollisionContext.Owner = m_pOwner;  m_CollisionContext.EventTag = {}; };
    void Reset_ContextEvent() { m_CollisionContext.EventTag = {}; };
    void Reset_ContextOwner() { m_CollisionContext.Owner = m_pOwner; };

public:
    void Render_GUI();
    virtual void Set_CompActive(_bool bActive) override;
    virtual void Releas_Component() override;

protected:
    _bool Compare_Same(COLLIDER_SLOT* prev, COLLIDER_SLOT* current);

protected:
    _int m_SystemIndex = { -1 };
    COLLISION_CONTEXT m_CollisionContext = {};
    unordered_set<COLLIDER_SLOT*> m_prevCollider = {}; //ÀÌÀü ÇÁ·¹ÀÓ¿¡ ºÎµúÈû
    unordered_set<COLLIDER_SLOT*> m_CurrentCollider = {}; //ÇöÀç ÇÁ·¹ÀÓ¿¡ ºÎµúÈû.

public:
    virtual void Free() override;
};

NS_END
