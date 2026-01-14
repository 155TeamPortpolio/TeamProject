#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CThugAssaulter;

class CThugAssaulter_Idle : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Idle* Create() { return new CThugAssaulter_Idle(); }
	virtual void Free() override { __super::Free(); }
};

NS_END