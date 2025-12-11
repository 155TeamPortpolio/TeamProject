#pragma once
#include "ICollisionService.h"

NS_BEGIN(Engine)

class CCollisionSystem 
    : public ICollisionService
{
private:
    CCollisionSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CCollisionSystem() DEFAULT;

public:
    HRESULT Initialize();
    virtual void Update(_float dt) override;
    virtual void Late_Update(_float dt) override;

public:
   virtual _int RegisterCollider(class CCollider* pCollider, _int Index)override;
   virtual void UnregisterCollider(class CCollider* pCollider, _int Index)override;
   virtual void DeActiveCollider(class CCollider* pCollider, _int Index) override;
   virtual void ActiveCollider(class CCollider* pCollider, _int Index) override;

private:
    void MakeCandidate();
    void Clean_Up();

private:
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = { nullptr };
    vector<COLLIDER_SLOT> m_Colliders;
    vector<pair<_int,_int>> m_CandidateCollision;


#ifdef _DEBUG
    virtual void Render_Debug()override;
private:
    ID3D11InputLayout* m_pInputLayout = { nullptr };
    BasicEffect* m_pEffect = { nullptr };
    PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
#endif 

public:
    static CCollisionSystem* Create(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END