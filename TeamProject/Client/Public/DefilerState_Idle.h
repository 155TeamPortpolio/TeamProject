#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CDefiler;
class CDefilerState_Idle : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

private:
	_float m_IdleElapsedTime = {};
	_float IdleDuration = {1.5f};
public:
	static CDefilerState_Idle* Create() { return new CDefilerState_Idle(); }
	virtual void Free() override { __super::Free(); }
};

NS_END