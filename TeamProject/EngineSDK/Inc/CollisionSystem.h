#pragma once
#include "ICollisionService.h"

NS_BEGIN(Engine)

class CCollisionSystem final : public ICollisionService
{
private:
    // Collider/RigidBody용 프록시
    class CPhysXEventCallback : public PxSimulationEventCallback
    {
    public:
        CPhysXEventCallback(CCollisionSystem* pSystem) : m_pOwner(pSystem) {}
        virtual ~CPhysXEventCallback() = default;

        virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
        virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
        virtual void onConstraintBreak(PxConstraintInfo*, PxU32) override {}
        virtual void onWake(PxActor**, PxU32) override {}
        virtual void onSleep(PxActor**, PxU32) override {}
        virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override {}

    private:
        CCollisionSystem* m_pOwner = nullptr;
    };

    // CCT용 프록시
    class CCCTHitCallback : public PxUserControllerHitReport
    {
    public:
        CCCTHitCallback(CCollisionSystem* pSystem) : m_pOwner(pSystem) {}
        virtual ~CCCTHitCallback() = default;

        virtual void onShapeHit(const PxControllerShapeHit& hit) override;
        virtual void onControllerHit(const PxControllersHit& hit) override;
        virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;

    private:
        CCollisionSystem* m_pOwner = nullptr;
    };

private:
    CCollisionSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CCollisionSystem() DEFAULT;

public:
    HRESULT Initialize();
    virtual void Update(_float dt) override;
    virtual void Late_Update(_float dt) override;
    void Render_GUI() override;

    PxUserControllerHitReport* Get_CCTCallback() override { return m_pCCTCallback; }

private:
    void Process_Contact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
    void Process_Trigger(PxTriggerPair* pairs, PxU32 count);
    void Process_CCT_ShapeHit(const PxControllerShapeHit& hit);
    void Process_CCT_ControllerHit(const PxControllersHit& hit);
    void Process_CCT_ObstacleHit(const PxControllerObstacleHit& hit);
    void Process_CollisionEvents();

    ICollidable* Get_Collidable_Actor(PxRigidActor* pActor);
    ICollidable* Get_Collidable_Shape(PxShape* pShape, PxRigidActor* pActor);

    friend class CPhysXEventCallback;
    friend class CCCTHitCallback;

public:
    virtual _int RegisterCollidable(class ICollidable* pCollidable, _int Index) override;
    virtual void UnRegisterCollidable(class ICollidable* pCollidable, _int Index) override;
#ifdef _DEBUG
    virtual void Render_Debug() override;

private:
    ID3D11InputLayout* m_pInputLayout = { nullptr };
    BasicEffect* m_pEffect = { nullptr };
    PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
#endif 

private:
    ID3D11Device*               m_pDevice = {nullptr};
    ID3D11DeviceContext*        m_pContext = { nullptr };
    CPhysXEventCallback*        m_pPhysXCallback = { nullptr }; 
    CCCTHitCallback*            m_pCCTCallback = { nullptr };
    vector<class ICollidable*>  m_Collidables;

public:
    static CCollisionSystem* Create(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END
