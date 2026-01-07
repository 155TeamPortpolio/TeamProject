#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;

class CJaneDoeState_Move final : public IHState<CJaneDoe>
{
public:
	virtual ~CJaneDoeState_Move() DEFAULT;

public:
	virtual void  Enter(CJaneDoe* pOwner) override;
	virtual void  Update(CJaneDoe* pOwner, _float dt) override;
	virtual void  Exit(CJaneDoe* pOwner) override {}

public:
	static CJaneDoeState_Move* Create() { return new CJaneDoeState_Move(); }
	virtual void Free() override { __super::Free(); }
};

NS_END