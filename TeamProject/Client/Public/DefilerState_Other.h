#pragma once
#include "IHState.h"
#include "Defiler_Control.h"

NS_BEGIN(Client)

class CDefiler;
class CDefilerState_Groggy : public IHState<CDefiler>
{
	enum state{GroggyIn, GroggyLoop, GroggyEnd};
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

private:
	state m_GroggyState = {};
public:
	static CDefilerState_Groggy* Create() { return new CDefilerState_Groggy(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Death : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;
public:
	static CDefilerState_Death* Create() { return new CDefilerState_Death(); }
	virtual void Free() override { __super::Free(); }
};


NS_END