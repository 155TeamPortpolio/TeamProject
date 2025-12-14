#pragma once
#include "ICollisionService.h"

NS_BEGIN(Engine)

class CCollisionSystem final : public ICollisionService
{
private:
    // Proxy Class : PhysX 이벤트를 수신하는 내부 클래스
    class CPhysXEventCallback : public PxSimulationEventCallback
    {
    public:
        CPhysXEventCallback(CCollisionSystem* pSystem) : m_pOwner(pSystem) {}
        virtual ~CPhysXEventCallback() = default;

        virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
        virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

        // 사용 안 함
        virtual void onConstraintBreak(PxConstraintInfo*, PxU32) override {}
        virtual void onWake(PxActor**, PxU32) override {}
        virtual void onSleep(PxActor**, PxU32) override {}
        virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override {}

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

public:
    // 디버그 렌더링을 위한 등록
   virtual _int RegisterCollider(class CCollider* pCollider, _int Index)override;
   virtual void UnregisterCollider(class CCollider* pCollider, _int Index)override;

private: // 내부 로직 (Proxy가 호출함)
    void Process_Contact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
    void Process_Trigger(PxTriggerPair* pairs, PxU32 count);
    friend class CPhysXEventCallback;   // Proxy 클래스


#ifdef _DEBUG
    virtual void Render_Debug()override;
private:
    ID3D11InputLayout* m_pInputLayout = { nullptr };
    BasicEffect* m_pEffect = { nullptr };
    PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
    vector<class CCollider*> m_Colliders; // 디버그 렌더링용
#endif 

private:
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = { nullptr };
    CPhysXEventCallback* m_pPhysXCallback = { nullptr };    // 이벤트 리스너

public:
    static CCollisionSystem* Create(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END

#pragma region OLD
//private:
//    void MakeCandidate();
//    void Clean_Up();
#pragma endregion