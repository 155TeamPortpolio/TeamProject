#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLevelObject abstract
	: public CGameObject
{
protected:
	CLevelObject();
	CLevelObject(const CLevelObject& rhs);
	virtual ~CLevelObject() DEFAULT;

public:
	void Set_Owner(class CLevel* pLevel) { m_pOwner = pLevel; }
protected:
	class CLevel* m_pOwner = { nullptr };//Week
public:
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr)PURE;
	virtual void Free() override;
};
NS_END
