#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CClaymore;

class CClaymore_Born : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Born* Create() { return new CClaymore_Born(); }
	virtual void Free() override { __super::Free(); }
};

NS_END