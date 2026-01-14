#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CThugAssaulter;

class CThugAssaulter_Born : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Born* Create() { return new CThugAssaulter_Born(); }
	virtual void Free() override { __super::Free(); }
};

NS_END