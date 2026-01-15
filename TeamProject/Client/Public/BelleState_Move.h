#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CBelle;
class CBelleState_Move :
    public IHState<CBelle>
{
public:
	virtual ~CBelleState_Move() DEFAULT;

public:
	virtual void  Enter(CBelle* pOwner) override;
	virtual void  Update(CBelle* pOwner, _float dt) override;
	virtual void  Exit(CBelle* pOwner) override {}

public:
	static CBelleState_Move* Create() { return new CBelleState_Move(); }
	virtual void Free() override { __super::Free(); }
};

NS_END