#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CThugPoacher;

class CThugPoacher_Born : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Born* Create() { return new CThugPoacher_Born(); }
	virtual void Free() override { __super::Free(); }
};

NS_END