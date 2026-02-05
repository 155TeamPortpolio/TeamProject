#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CStrikeJaeger;

class CStrikeJaeger_Parried : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Parried* Create() { return new CStrikeJaeger_Parried(); }
	virtual void Free() override { __super::Free(); }
};

NS_END