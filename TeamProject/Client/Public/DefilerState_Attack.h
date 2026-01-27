#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CDefiler;
class CDefilerState_Attack : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack* Create();
	virtual void Free() override { __super::Free(); }
};

NS_END