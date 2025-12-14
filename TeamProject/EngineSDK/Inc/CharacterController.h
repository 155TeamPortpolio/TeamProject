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
    void            Set_PlanarVelocity(_fvector vVelocity);
    _float3         Get_Velocity() const { return m_vVelocity; }
    _float          Get_Height() const { return m_fHeight; }
    _float          Get_Radius() const { return m_fRadius; }


public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;
    
    virtual void    OnCollisionEnter(ICollidable* pOther) override;
    virtual void    OnCollisionStay(ICollidable* pOther) override;
    virtual void    OnCollisionExit(ICollidable* pOther) override;
    
    void            Update(_float dt);
    void            Late_Update(_float dt);
    // GUI 렌더링
    void            Render_GUI();

#ifdef _DEBUG
    virtual void Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

public:
    void            Move(_fvector vDisp, _float dt);
    void            Jump(_float fJump);
    void            Set_Position(_fvector vPos);
    void            Resize(_float fHeight, _float fRadius);
    _vector         Get_FootPosition();


private:
    void Apply_Gravity(_float dt);
    void Apply_Move(_float dt);
    // 내부 로직 (Proxy가 호출함)
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
    CCTHitReportProxy*       m_pHitReport = { nullptr }; // 프록시 객체
    PxFilterData             m_FilterData = {};

    _bool                    m_bGrounded = { false };
    _float3                  m_vVelocity = { 0.f, 0.f, 0.f };
    _float                   m_fHeight = { 0.f };
    _float                   m_fRadius = { 0.f };

public:
    static CCharacterController* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;

};

NS_END