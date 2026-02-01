#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CClaymore;

class CClaymore_Parried : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Parried* Create() { return new CClaymore_Parried(); }
	virtual void Free() override { __super::Free(); }
};

NS_END