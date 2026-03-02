#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiasmaGrandierJaeger;
class CMiasmaHeavyJaeger
	;
class CMiasmaGrandierJaeger_Appear : public IHState<CMiasmaGrandierJaeger>
{
public:
	virtual void Enter(CMiasmaGrandierJaeger* pOwner) override;
	virtual void Update(CMiasmaGrandierJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaGrandierJaeger* pOwner) override;

private:
	_float m_Duration = {};
	_float m_AppearTime = {};

public:
	static CMiasmaGrandierJaeger_Appear* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaGrandierJaeger_Attack : public IHState<CMiasmaGrandierJaeger>
{
public:
	virtual void Enter(CMiasmaGrandierJaeger* pOwner) override;
	virtual void Update(CMiasmaGrandierJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaGrandierJaeger* pOwner) override;

public:
	static CMiasmaGrandierJaeger_Attack* Create();
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CMiasmaGrandierJaeger* pOwner);
};

class CMiasmaGrandierJaeger_Hit : public IHState<CMiasmaGrandierJaeger>
{
public:
	virtual void Enter(CMiasmaGrandierJaeger* pOwner) override;
	virtual void Update(CMiasmaGrandierJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaGrandierJaeger* pOwner) override;

public:
	static CMiasmaGrandierJaeger_Hit* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaGrandierJaeger_DisAppear : public IHState<CMiasmaGrandierJaeger>
{
public:
	virtual void Enter(CMiasmaGrandierJaeger* pOwner) override;
	virtual void Update(CMiasmaGrandierJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaGrandierJaeger* pOwner) override;

public:
	static CMiasmaGrandierJaeger_DisAppear* Create();
	virtual void Free() override { __super::Free(); }

private:
	void Spawn_Effect(CMiasmaGrandierJaeger* pOwner);
};

//=================================================================================//
class CMiasmaHeavyJaeger_Appear : public IHState<CMiasmaHeavyJaeger>
{
public:
	virtual void Enter(CMiasmaHeavyJaeger* pOwner) override;
	virtual void Update(CMiasmaHeavyJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaHeavyJaeger* pOwner) override;

private:
	_float m_Duration = {};
	_float m_AppearTime = {};

public:
	static CMiasmaHeavyJaeger_Appear* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaHeavyJaeger_Attack1 : public IHState<CMiasmaHeavyJaeger>
{
public:
	virtual void Enter(CMiasmaHeavyJaeger* pOwner) override;
	virtual void Update(CMiasmaHeavyJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaHeavyJaeger* pOwner) override;

public:
	static CMiasmaHeavyJaeger_Attack1* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaHeavyJaeger_Attack2 : public IHState<CMiasmaHeavyJaeger>
{
public:
	virtual void Enter(CMiasmaHeavyJaeger* pOwner) override;
	virtual void Update(CMiasmaHeavyJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaHeavyJaeger* pOwner) override;

public:
	static CMiasmaHeavyJaeger_Attack2* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaHeavyJaeger_DisAppear : public IHState<CMiasmaHeavyJaeger>
{
public:
	virtual void Enter(CMiasmaHeavyJaeger* pOwner) override;
	virtual void Update(CMiasmaHeavyJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaHeavyJaeger* pOwner) override;

public:
	static CMiasmaHeavyJaeger_DisAppear* Create();
	virtual void Free() override { __super::Free(); }
};

NS_END