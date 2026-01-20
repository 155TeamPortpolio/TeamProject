#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Born : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Born* Create() { return new CSacrificeState_Born(); }
	virtual void Free() override { __super::Free(); }

};

/* Sub States */
class CSacrificeState_Born_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Born_Phase1* Create() { return new CSacrificeState_Born_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrifice* pOwner);
	_bool m_IsEffectSpawn = false;

};

class CSacrificeState_Born_Phase2 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Born_Phase2* Create() { return new CSacrificeState_Born_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

NS_END