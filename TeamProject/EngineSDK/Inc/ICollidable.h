#pragma once
#include "Engine_Defines.h"
#include "Component.h"

class ENGINE_DLL ICollidable abstract : public CComponent
{
protected:
    ICollidable() = default;
	ICollidable(const ICollidable& rhs) : CComponent(rhs) {};
    virtual ~ICollidable() = default;

public:
    virtual void OnCollisionEnter(ICollidable* pOther) = 0;
    virtual void OnCollisionStay(ICollidable* pOther) = 0;
    virtual void OnCollisionExit(ICollidable* pOther) = 0;

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