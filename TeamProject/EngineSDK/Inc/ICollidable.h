#pragma once
#include "Engine_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL ICollidable abstract : public CComponent
{
protected:
    ICollidable() = default;
	ICollidable(const ICollidable& rhs) : CComponent(rhs) {};
    virtual ~ICollidable() = default;

public:
    virtual void OnCollisionEnter(ICollidable* pOther) PURE;
    virtual void OnCollisionStay(ICollidable* pOther) PURE;
    virtual void OnCollisionExit(ICollidable* pOther) PURE;
	virtual void OnTriggerEnter(ICollidable* pOther) PURE;
	virtual void OnTriggerExit(ICollidable* pOther) PURE;

#ifdef _DEBUG
	virtual void Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) PURE;
#endif
	_bool IsColliding() const { return !m_CurrentCollisions.empty(); }
	_bool IsCollidingWith(ICollidable* pOther) const
	{
		return m_CurrentCollisions.find(pOther) != m_CurrentCollisions.end();
	}
	unordered_set<ICollidable*>& Get_Collisions() { return m_CurrentCollisions; }

protected:
	unordered_set<ICollidable*> m_CurrentCollisions;

public:
	virtual void Free() override { __super::Free(); }
};

NS_END