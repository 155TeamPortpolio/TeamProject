#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CGiant;

class CClaymore_Idle : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Idle* Create() { return new CClaymore_Idle(); }
	virtual void Free() override { __super::Free(); }
};

NS_END