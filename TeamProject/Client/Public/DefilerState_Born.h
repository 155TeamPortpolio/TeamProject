#pragma once
#include "IHState.h"
#include "Defiler_Control.h"

NS_BEGIN(Client)
class CDefiler;
class CDefilerState_Born : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;
private:
	_bool m_bBorn = false;
	_float m_ElapsedTime = {};
public:
	static CDefilerState_Born* Create() { return new CDefilerState_Born(); }
	virtual void Free() override { __super::Free(); }
};

NS_END