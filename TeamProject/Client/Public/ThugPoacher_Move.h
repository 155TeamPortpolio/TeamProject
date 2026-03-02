#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugPoacher;

class CThugPoacher_Move : public IHState<CThugPoacher>
{
private:
	enum MOVEINDEX : _int {
		Walk_Front = 1,
		Walk_Back,
		Walk_Left,
		Walk_Right,
		Walk_FL_RFoot,
		Walk_FR_RFoot,
		Walk_RF_LFoot,
		Evade
	};

public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Move* Create() { return new CThugPoacher_Move(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void ChangeMovePatternFromIndex(_int iMoveIndex);
};

class CThugPoacher_Walk_Front : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Walk_Front* Create() { return new CThugPoacher_Walk_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Walk_Back : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Walk_Back* Create() { return new CThugPoacher_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Walk_Left : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Walk_Left* Create() { return new CThugPoacher_Walk_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Walk_Right : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Walk_Right* Create() { return new CThugPoacher_Walk_Right(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Walk_FL_RFoot : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Walk_FL_RFoot* Create() { return new CThugPoacher_Walk_FL_RFoot(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Walk_FR_RFoot : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Walk_FR_RFoot* Create() { return new CThugPoacher_Walk_FR_RFoot(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Walk_RF_LFoot : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Walk_RF_LFoot* Create() { return new CThugPoacher_Walk_RF_LFoot(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Walk_Evade : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Walk_Evade* Create() { return new CThugPoacher_Walk_Evade(); }
	virtual void Free() override { __super::Free(); }
};


NS_END