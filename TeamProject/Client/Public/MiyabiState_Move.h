#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_Move final :  public IHState<CMiyabi>
{
public:
	virtual ~CMiyabiState_Move() DEFAULT;

public:
	virtual void  Enter(CMiyabi* pOwner) override;
	virtual void  Update(CMiyabi* pOwner, _float dt) override;
	virtual void  Exit(CMiyabi* pOwner) override;

public:
	static CMiyabiState_Move* Create();
	virtual void Free() override { __super::Free(); }
};

NS_END