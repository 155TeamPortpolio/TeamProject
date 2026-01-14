#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugAssaulter;

class CThugAssaulter_Move : public IHState<CThugAssaulter>
{
private:
	enum MOVEINDEX : _int {
		Walk_Front = 1,
		Walk_Back,
		Walk_Left,
		Walk_Right,
		Walk_FL_LFoot,
		Walk_FR_LFoot,
		Walk_LF_RFoot,
		Evade
	};
	
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Move* Create() { return new CThugAssaulter_Move(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void ChangeMovePatternFromIndex(_int iMoveIndex);
};

class CThugAssaulter_Walk_Front : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Walk_Front* Create() { return new CThugAssaulter_Walk_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Walk_Back : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Walk_Back* Create() { return new CThugAssaulter_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Walk_Left : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Walk_Left* Create() { return new CThugAssaulter_Walk_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Walk_Right : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Walk_Back* Create() { return new CThugAssaulter_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Walk_FL_LFoot : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Walk_Back* Create() { return new CThugAssaulter_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Walk_FR_LFoot : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Walk_Back* Create() { return new CThugAssaulter_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Walk_LF_RFoot : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Walk_Back* Create() { return new CThugAssaulter_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Walk_Evade : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Walk_Evade* Create() { return new CThugAssaulter_Walk_Evade(); }
	virtual void Free() override { __super::Free(); }
};


NS_END