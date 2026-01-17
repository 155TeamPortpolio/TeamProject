#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_Move final : public IHState<CCorin>
{
public:
	virtual ~CCorinState_Move() DEFAULT;

public:
	virtual void  Enter(CCorin* pOwner) override;
	virtual void  Update(CCorin* pOwner, _float dt) override;
	virtual void  Exit(CCorin* pOwner) override;

public:
	static CCorinState_Move* Create() { return new CCorinState_Move(); }
	virtual void Free() override { __super::Free(); }
};

NS_END