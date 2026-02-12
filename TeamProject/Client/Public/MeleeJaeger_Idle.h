#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMeleeJaeger;

class CMeleeJaeger_Idle : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Idle* Create() { return new CMeleeJaeger_Idle(); }
	virtual void Free() override { __super::Free(); }
};

NS_END