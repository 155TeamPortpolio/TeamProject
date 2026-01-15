#pragma once
#include "ICollidable.h"
#include "RigidBody.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public ICollidable
{
private:
    CCollider();
    CCollider(const CCollider& rhs);
    virtual ~CCollider() DEFAULT;

public:
    PxRigidActor* Get_PxActor()
    {
        // 리지드바디가 붙어있다면 동적 Actor 반환
        if (m_pAttachedRigidBody)
            return m_pAttachedRigidBody->Get_Body();
        // 없다면 정적 Actor 반환 (Static Collider)
        return m_pStaticActor;
    }
    PxShape*        Get_Shape() { return m_pShape; }
    _bool           Is_Trigger() const { return m_bTrigger; }
    COLLIDER_TYPE   Get_Type() const { return m_eType; }
    _float3         Get_Center() const { return m_vCenter; }
    _float3         Get_Size() const { return m_vSize; }
    _float3         Get_Rotation() const { return m_vRotation; }
    void            Set_MapToolMode(_bool bEnable) { m_bMapTool = bEnable; }
    _bool           Is_MapToolMode() const { return m_bMapTool; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;
    void            Update(_float dt);
    void            Render_GUI();
    // 충돌 이벤트 핸들러 (System에서 호출)
    virtual void    OnCollisionEnter(ICollidable* pOther) override;
    virtual void    OnCollisionStay(ICollidable* pOther) override;
    virtual void    OnCollisionExit(ICollidable* pOther) override;
    void            OnTriggerEnter(ICollidable* pOther) override;
    void            OnTriggerStay(ICollidable* pOther) override;
    void            OnTriggerExit(ICollidable* pOther) override;

private:
    _bool           m_bDebugRender = { true };
public:
    virtual void    Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
    _bool           IsDebugRender() { return m_bDebugRender; }
    void            Set_DebugRender(_bool bDebugRender) { m_bDebugRender = bDebugRender; }


public:
    void            Set_Center(const _float3& vCenter);
    void            Set_Size(const _float3& vSize);
    void            Set_Rotation(const _float3& vRotation);
    void            Set_Trigger(_bool bTrigger);
    void            Set_ContactOffset(_float fOffset);  // 충돌계산시작 버퍼 구간 : 크면 허공충돌, 작으면 터널링/겹침
    void            Set_RestOffset(_float fOffset);     // 정지했을때 서로 유지하려는 거리 : 양수일때 바깥충돌, 음수일때 안쪽충돌
    void            Set_CollisionMask(_uint iMask);
    void            Set_CollisionGroup(COLLISION_GROUP eGroup);

private:
    void            Update_LocalPose();
    HRESULT         AutoFit(COLLIDER_DESC* pDesc);
    void            Sync_Transform();

private:
    class IPhysicsService*      m_pPhysicsSystem = { nullptr };
    class CTransform*           m_pOwnerTransform = { nullptr };
    PxShape*                    m_pShape = { nullptr };
    class CRigidBody*           m_pAttachedRigidBody = { nullptr };
    PxRigidStatic*              m_pStaticActor = { nullptr };

    PxTriangleMesh*             m_pTriangleMesh = { nullptr };  // 쿠킹된 메쉬
    _bool                       m_bCooked = { false };      // 쿠킹 여부
    COLLIDER_TYPE               m_eType = {};
    COLLISION_GROUP             m_eGroup = { COLLISION_GROUP::COMMON };
    _uint                       m_iCollisionMask = { 0xFFFFFFFF };
    _float3                     m_vCenter = {};
    _float3                     m_vSize = {};
    _float3                     m_vRotation = {};
    _bool                       m_bTrigger = {};
    string                      m_strMaterialTag = {};

    _bool                       m_bMapTool = { false };


public:
    static CCollider* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END
