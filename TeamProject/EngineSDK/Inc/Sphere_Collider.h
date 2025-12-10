#pragma once
#include "Collider.h"
NS_BEGIN(Engine)
class ENGINE_DLL CSphere_Collider :
    public CCollider
{
private:
    CSphere_Collider();
    CSphere_Collider(const CSphere_Collider& rhs);
    virtual ~CSphere_Collider() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;
public:
    virtual void Update() override;
    virtual void Late_Update() override;
    virtual _bool Intersect(COLLIDER_SLOT* pSlot) override;
    const BoundingSphere* Get_Desc() const {
        return m_pDesc;
    }
public:
    virtual COLLIDER_TYPE Get_ColliderType() override { return COLLIDER_TYPE::SPHERE; };
    virtual void Make_MinMaxCollider(MINMAX_BOX minMax)override;
    virtual _bool Has_Desc() override { return (m_pDesc && m_pOriginalDesc); };

public:
#ifdef _DEBUG
    virtual void Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif
private:
    BoundingSphere* m_pDesc = {nullptr};
    BoundingSphere* m_pOriginalDesc = {nullptr};

public:
    static CSphere_Collider* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};
NS_END
