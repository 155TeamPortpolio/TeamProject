#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Groggy : IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Groggy* Create() { return new CSacrificeState_Groggy(); }
	virtual void Free() override { __super::Free(); }

};

/* Sub States */

class CSacrificeState_Groggy_Phase1 : IBaseState<CSacrifice>
{
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Groggy_Phase1* Create() { return new CSacrificeState_Groggy_Phase1(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Groggy_Phase2 : IBaseState<CSacrifice>
{
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Groggy_Phase2* Create() { return new CSacrificeState_Groggy_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

NS_END
