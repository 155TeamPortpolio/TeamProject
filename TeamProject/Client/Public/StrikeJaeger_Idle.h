#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CStrikeJaeger;

class CStrikeJaeger_Idle : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Idle* Create() { return new CStrikeJaeger_Idle(); }
	virtual void Free() override { __super::Free(); }
};

NS_END