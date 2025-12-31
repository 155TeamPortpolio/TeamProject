#pragma once
#include "ICollidable.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCharacterController final : public ICollidable
{
private:
    // CCT Query Filter Callback 내부 클래스
    class CCCTQueryFilter : public PxQueryFilterCallback
    {
    public:
        CCCTQueryFilter(PxFilterData* pFilterData) : m_pFilterData(pFilterData) {}
        virtual ~CCCTQueryFilter() = default;

        virtual PxQueryHitType::Enum preFilter(
            const PxFilterData& filterData,
            const PxShape* shape,
            const PxRigidActor* actor,
            PxHitFlags& queryFlags) override
        {
            if (!m_pFilterData || !shape)
                return PxQueryHitType::eBLOCK;

            // Shape에서 직접 QueryFilterData 가져오기
            // (filterData 인자는 mFilterData가 전달되므로 사용하지 않음)
            PxFilterData shapeFilterData = shape->getQueryFilterData();

            PxU32 myGroup = m_pFilterData->word0;
            PxU32 myMask = m_pFilterData->word1;
            PxU32 otherGroup = shapeFilterData.word0;
            PxU32 otherMask = shapeFilterData.word1;

            // 양방향 검사
            _bool bIAllowOther = (myMask & otherGroup) != 0;
            _bool bOtherAllowMe = (otherMask & myGroup) != 0;

            if (!bIAllowOther || !bOtherAllowMe)
                return PxQueryHitType::eNONE;

            return PxQueryHitType::eBLOCK;
        }

        virtual PxQueryHitType::Enum postFilter(
            const PxFilterData& filterData,
            const PxQueryHit& hit) override
        {
            return PxQueryHitType::eBLOCK;
        }

    private:
        PxFilterData* m_pFilterData = nullptr;
    };

private:
    CCharacterController() {}
    CCharacterController(const CCharacterController& rhs) :ICollidable(rhs) {}
    virtual ~CCharacterController() DEFAULT;

public:
    PxController*   Get_Controller() { return m_pController; }
    PxShape*        Get_Shape();
    _bool           Is_Grounded() const { return m_bGrounded; }
    _float3         Get_Velocity() const { return m_vVelocity; }
    _float          Get_Height() const { return m_fHeight; }
    _float          Get_Radius() const { return m_fRadius; }
    _float          Get_MaxSpeed() const { return m_fMaxSpeed; }
    void            Set_Gravity(_float fGravity) { m_fGravity = fGravity; }
    _uint           Get_CollisionMask() const { return m_FilterData.word1; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;
    
    virtual void    OnCollisionEnter(ICollidable* pOther) override;
    virtual void    OnCollisionStay(ICollidable* pOther) override;
    virtual void    OnCollisionExit(ICollidable* pOther) override;
    virtual void    OnTriggerEnter(ICollidable* pOther) override;
    virtual void    OnTriggerExit(ICollidable* pOthter) override;

    void            Update(_float dt);
    void            Late_Update(_float dt);
    void            Render_GUI();       // GUI 렌더링

    void            Process_Response(const PxControllerShapeHit& hit);
    virtual void    Update_Collisions() override {
        m_PreviousCollisions = m_CurrentCollisions;
        m_CurrentCollisions.clear();
    }

    virtual void    Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
    void            Render_DebugRay(PrimitiveBatch<VertexPositionColor>* pBatch);


public:
    void            Move_Direction(_fvector vDir, _float fSpeed, _float dt);
    void            Move_Velocity(_fvector vVelocity, _float dt);
    void            Move_Displacement(_fvector vDisp, _float dt);
    void            Move_RootMotion(_fvector vLocalDelta, _fvector qRotation, _float dt);
    void            Stop_Movement();
    void            Jump(_float fJumpSpeed);
    void            Set_Position(_fvector vPos);
    void            Resize(_float fHeight, _float fRadius);
    void            Set_StepOffset(_float fOffset);
    void            Set_SlopeLimit(_float fDegree);
    void            Set_Velocity(_fvector vVelocity);
    void            Set_PlanarVelocity(_fvector vVelocity);
    void            Set_VerticalVelocity(_float fVelocity);
    void            Set_MaxSpeed(_float fMaxSpeed);
    void            Set_GravityEnabled(_bool bEnabled);
    void            Set_FootPosition(_vector3 vFootPos);
    _vector         Get_FootPosition();
    void            Set_ContactOffset(_float fOffset);
    void            Set_RestOffset(_float fOffset);
    _float          Get_ContactOffset();
    _float          Get_RestOffset();
    void            Set_BoundingMinY(_float fMinY);

    _bool           Shoot_Ray(_fvector vDirection, _float fDistance, PHYSICS_RAY_HIT& hit);
    void            Clear_DebugRay() { m_bShowDebugRay = false; m_DebugRayHit.bHit = false; }

    _float          Get_HalfSize()  const { return m_fHeight * 0.5f + m_fRadius * 0.5f; }

    void            Set_CollisionMask(_uint iMask);
    void            Set_CollisionGroup(COLLISION_GROUP eGroup);

private:
    void            Move(_fvector vDisplacement, _float dt);
    void            Apply_Gravity(_float dt);
    HRESULT         AutoFit(CCT_DESC* pDesc);

private:
    class IPhysicsService*   m_pPhysicsSystem = { nullptr };
    class CTransform*        m_pOwnerTransform = { nullptr };
    PxController*            m_pController = { nullptr };
    PxControllerManager*     m_pManager = { nullptr };
    PxMaterial*              m_pMaterial = { nullptr };
    PxFilterData             m_FilterData = {};
    PxFilterData             m_QueryFilterData = {};
    CCCTQueryFilter*         m_pQueryFilter = { nullptr };
    _bool                    m_bGrounded = { false };
    _bool                    m_bGravityEnabled = { true };
    _vector3                 m_vVelocity = { 0.f, 0.f, 0.f };
    _vector3                 m_vPrevVelocity = { 0.f, 0.f, 0.f };
    _float                   m_fHeight = { 0.f };
    _float                   m_fRadius = { 0.f };
    _float                   m_fStepOffset = { 0.5f };
    _float                   m_fSlopeLimit = { 45.0f };
    _float                   m_fMaxSpeed = { 0.0f };
    _float                   m_fGravity = { -9.81f };
    _float                   m_fContactOffset = 0.001f;
    _float                   m_fFootOffset = { 0.f };
    _float                   m_fRestOffset = 0.0f;
    _float                   m_fBoundingMinY = 0.f;
    _bool                    m_bShowDebugRay = { false };
    PHYSICS_RAY_HIT          m_DebugRayHit = {};

    // 디버그 레이 시각화용
    _float3                  m_vRayStart = {};
    _float3                  m_vRayEnd = {};

public:
    static CCharacterController* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END