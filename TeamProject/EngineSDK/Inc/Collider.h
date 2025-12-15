#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
    CCollider();
    CCollider(const CCollider& rhs);
    virtual ~CCollider() DEFAULT;

public:
    PxShape* Get_Shape() { return m_pShape; }
    _bool IsColliding() const { return !m_CurrentCollisions.empty(); }


public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;
    // 충돌 이벤트 핸들러 (System에서 호출)
    void OnCollisionEnter(CCollider* pOther, const PxContactPair& contactInfo);
    void OnCollisionExit(CCollider* pOther);
    void OnTriggerEnter(CCollider* pOther);
    void OnTriggerExit(CCollider* pOther);

    void Render_GUI();

#ifdef _DEBUG
    void Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor);
#endif

private:
    PxShape*                    m_pShape = { nullptr };
    class CRigidBody*           m_pAttachedRigidBody = { nullptr };
    unordered_set<CCollider*>   m_CurrentCollisions;     // 현재 충돌 중인 대상 목록 (Stay 처리나 중복 Enter 방지용)

    COLLIDER_TYPE               m_eType = {};
    _float3                     m_vCenter = {};
    _float3                     m_vSize = {};
    _float3                     m_vRotation = {};
    _bool                       m_bTrigger = {};
    string                      m_strMaterialTag = {};

public:
    static CCollider* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END


#pragma region OLD
//public:
//   virtual _bool Intersect(COLLIDER_SLOT* pSlot) PURE;
//   virtual _bool Has_Desc() PURE;
//   virtual void Make_MinMaxCollider(MINMAX_BOX minMax)PURE;
//   virtual void Set_ColliderActive(_bool Active);

//public:
//    virtual COLLIDER_TYPE Get_ColliderType() PURE;
//    COLLISION_CONTEXT Get_Context() { return m_CollisionContext; };
//    void Set_Context(class CGameObject* pObject,const string& eventTag) { m_CollisionContext.Owner = pObject; m_CollisionContext.EventTag = eventTag;};
//    void Set_ContextOwner(class CGameObject* pObject) { m_CollisionContext.Owner = pObject; };
//    void Set_ContextEvent(const string& eventTag) { m_CollisionContext.EventTag = eventTag; };
//    void Reset_Context() { m_CollisionContext.Owner = m_pOwner;  m_CollisionContext.EventTag = {}; };
//    void Reset_ContextEvent() { m_CollisionContext.EventTag = {}; };
//    void Reset_ContextOwner() { m_CollisionContext.Owner = m_pOwner; };

//public:
//    void Render_GUI();
//    virtual void Set_CompActive(_bool bActive) override;
//    virtual void Releas_Component() override;

//protected:
//    _bool Compare_Same(COLLIDER_SLOT* prev, COLLIDER_SLOT* current);
//
//protected:
//    _int m_SystemIndex = { -1 };
//    COLLISION_CONTEXT m_CollisionContext = {};
//    unordered_set<COLLIDER_SLOT*> m_prevCollider = {}; //이전 프레임에 부딪힘
//    unordered_set<COLLIDER_SLOT*> m_CurrentCollider = {}; //현재 프레임에 부딪힘.
#pragma endregion