#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CMeleeJaeger;

class CMeleeJaeger_Parried : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Parried* Create() { return new CMeleeJaeger_Parried(); }
	virtual void Free() override { __super::Free(); }
};

NS_END