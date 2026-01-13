#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugAssaulter;

class CThugAssaulter_Death : public IHState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Death* Create() { return new CThugAssaulter_Death(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

//class CThugAssaulter_Attack1 : public IBaseState<CThugAssaulter>
//{
//public:
//	virtual void Enter(CThugAssaulter* pOwner) override;
//	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
//	virtual void Exit(CThugAssaulter* pOwner) override;
//
//public:
//	static CThugAssaulter_Attack1* Create() { return new CThugAssaulter_Attack1(); }
//	virtual void Free() override { __super::Free(); }
//};

NS_END