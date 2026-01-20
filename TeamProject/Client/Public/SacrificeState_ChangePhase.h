#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_ChangePhase : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_ChangePhase* Create() { return new CSacrificeState_ChangePhase(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrifice* pOwner);

};
NS_END
