#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCyclops;

class CCyclops_Born : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Born* Create() { return new CCyclops_Born(); }
	virtual void Free() override { __super::Free(); }
};

NS_END