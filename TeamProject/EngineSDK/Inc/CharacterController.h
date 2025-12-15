#pragma once
#include "ICollidable.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCharacterController final : public ICollidable
{
private:
    // [Proxy Class] PhysX 이벤트를 수신하는 내부 클래스
    class CCTHitReportProxy : public PxUserControllerHitReport
    {
    public:
        CCTHitReportProxy(CCharacterController* pOwner) : m_pOwner(pOwner) {}
        virtual ~CCTHitReportProxy() = default;

        // PxUserControllerHitReport 인터페이스 구현
        virtual void onShapeHit(const PxControllerShapeHit& hit) override;
        virtual void onControllerHit(const PxControllersHit& hit) override;
        virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;

    private:
        CCharacterController* m_pOwner = nullptr;
    };

private:
    CCharacterController();
    CCharacterController(const CCharacterController& rhs);
    virtual ~CCharacterController() override DEFAULT;

public:
    PxController*   Get_Controller() { return m_pController; }
    PxShape*        Get_Shape();
    _bool           Is_Grounded() const { return m_bGrounded; }
    _float3         Get_Velocity() const { return m_vVelocity; }
    _float          Get_Height() const { return m_fHeight; }
    _float          Get_Radius() const { return m_fRadius; }
    _float          Get_MaxSpeed() const { return m_fMaxSpeed; }
    void            Set_Gravity(_float fGravity) { m_fGravity = fGravity; }


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

#ifdef _DEBUG
    virtual void Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

public:
    void            Move_Direction(_fvector vDir, _float fSpeed);   // 일반적인 이동 : 방향 + 속도
    void            Move_Velocity(_fvector vVelocity);              // 속도벡터로 이동
    void            Stop_Movement();
    void            Move(_fvector vDisp, _float dt);                // 특수상황에만 사용 : 대시, 넉백
    void            Jump(_float fJump);
    void            Set_Position(_fvector vPos);                    // 텔레포트
    void            Resize(_float fHeight, _float fRadius);
    void            Set_StepOffset(_float fOffset);
    void            Set_SlopeLimit(_float fDegree);
    void            Set_PlanarVelocity(_fvector vVelocity);         // XZ
    void            Set_VerticalVelocity(_float fVelocity);         // Y
    void            Set_MaxSpeed(_float fMaxSpeed);
    void            Set_GravityEnabled(_bool bEnabled);
    _vector         Get_FootPosition();



private:
    void Apply_Gravity(_float dt);
    void Apply_Move(_float dt);
    void Process_ShapeHit(const PxControllerShapeHit& hit);
    void Process_ControllerHit(const PxControllersHit& hit);
    void Process_ObstacleHit(const PxControllerObstacleHit& hit);
    friend class CCTHitReportProxy;

private:
    class IPhysicsService*   m_pPhysicsSystem = { nullptr };
    class CTransform*        m_pOwnerTransform = { nullptr };
    PxController*            m_pController = { nullptr };
    PxControllerManager*     m_pManager = { nullptr };
    PxMaterial*              m_pMaterial = { nullptr };
    CCTHitReportProxy*       m_pHitReport = { nullptr };
    PxFilterData             m_FilterData = {};
    _bool                    m_bGrounded = { false };
    _bool                    m_bGravityEnabled = { true };
    _float3                  m_vVelocity = { 0.f, 0.f, 0.f };
    _float                   m_fHeight = { 0.f };
    _float                   m_fRadius = { 0.f };
    _float                   m_fStepOffset = { 0.5f };
    _float                   m_fSlopeLimit = { 45.0f };
    _float                   m_fMaxSpeed = { 0.0f };
    _float                   m_fGravity = { -9.81f };



public:
    static CCharacterController* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;

};

NS_END