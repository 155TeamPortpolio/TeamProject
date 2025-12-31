#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CAnbi;

class CAnbiState_Move final : public IHState<CAnbi>
{
public:
	virtual ~CAnbiState_Move() DEFAULT;

public:
	virtual void  Enter(CAnbi* pOwner) override;
	virtual void  Update(CAnbi* pOwner, _float dt) override;
	virtual void  Exit(CAnbi* pOwner) override {}
	virtual _bool Handle_Transition(CAnbi* pOwner, const string& strState) override;

public:
	static CAnbiState_Move* Create() { return new CAnbiState_Move(); }
	virtual void Free() override { __super::Free(); }
};

NS_END