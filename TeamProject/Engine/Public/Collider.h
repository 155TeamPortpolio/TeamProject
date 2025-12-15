#pragma once
#include "ICollidable.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public ICollidable
{
private:
    CCollider();
    CCollider(const CCollider& rhs);
    virtual ~CCollider() DEFAULT;

public:
    PxShape*        Get_Shape() { return m_pShape; }
    _bool           IsTrigger() const { return m_bTrigger; }
    COLLIDER_TYPE   Get_Type() const { return m_eType; }
    _float3         Get_Center() const { return m_vCenter; }
    _float3         Get_Size() const { return m_vSize; }
    _float3         Get_Rotation() const { return m_vRotation; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;
    // 충돌 이벤트 핸들러 (System에서 호출)
    virtual void    OnCollisionEnter(ICollidable* pOther) override;
    virtual void    OnCollisionStay(ICollidable* pOther) override;
    virtual void    OnCollisionExit(ICollidable* pOther) override;
    void            OnTriggerEnter(ICollidable* pOther);
    void            OnTriggerExit(ICollidable* pOther);

    void Render_GUI();

#ifdef _DEBUG
    virtual void Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

public:
    void            Set_Center(const _float3& vCenter);
    void            Set_Size(const _float3& vSize);
    void            Set_Rotation(const _float3& vRotation);
    void            Set_Trigger(_bool bTrigger);
    void            Set_ContactOffset(_float fOffset);
    void            Set_RestOffset(_float fOffset);

private:
    void            Update_LocalPose();

private:
    PxShape*                    m_pShape = { nullptr };
    class CRigidBody*           m_pAttachedRigidBody = { nullptr };
    COLLIDER_TYPE               m_eType = {};
    COLLISION_GROUP             m_eGroup = {};
    _uint                       m_iCollisionMask = {};
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
