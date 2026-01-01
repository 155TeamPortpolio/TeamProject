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

	virtual void Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) PURE;

	_bool IsColliding() const { return !m_CurrentCollisions.empty(); }
	_bool IsCollidingWith(ICollidable* pOther) const
	{
		return m_CurrentCollisions.find(pOther) != m_CurrentCollisions.end();
	}
	unordered_set<ICollidable*>& Get_CurrentCollisions() { return m_CurrentCollisions; }
	unordered_set<ICollidable*>& Get_PreviousCollisions() { return m_PreviousCollisions; }
	virtual void Update_Collisions() { m_PreviousCollisions = m_CurrentCollisions;}
	
	void Set_SlotInfo(_int iIndex, _uint iGeneration)
	{
		m_iSlotIndex = iIndex;
		m_iSlotGeneration = iGeneration;
	}
	_int  Get_SlotIndex() const { return m_iSlotIndex; }
	_uint Get_SlotGeneration() const { return m_iSlotGeneration; }

protected:
	unordered_set<ICollidable*> m_CurrentCollisions;
	unordered_set<ICollidable*> m_PreviousCollisions;
	_int  m_iSlotIndex = { -1 };
	_uint m_iSlotGeneration = { 0 };

public:
	virtual void Free() override { __super::Free(); }
};

NS_END