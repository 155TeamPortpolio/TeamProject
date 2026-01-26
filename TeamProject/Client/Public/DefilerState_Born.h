#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CDefiler;
class CDefilerState_Born : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Born* Create() { return new CDefilerState_Born(); }
	virtual void Free() override { __super::Free(); }

};

/* Sub States */
class CDefilerState_Born_Phase1 : public IBaseState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Born_Phase1* Create() { return new CDefilerState_Born_Phase1(); }
	virtual void Free() override { __super::Free(); }

};

class CDefilerState_Born_Phase2 : public IBaseState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Born_Phase2* Create() { return new CDefilerState_Born_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

NS_END