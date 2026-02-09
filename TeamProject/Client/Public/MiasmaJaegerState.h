#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiasmaGrandierJaeger;
class CMiasmaJaeger_Appear : public IHState<CMiasmaGrandierJaeger>
{
public:
	virtual void Enter(CMiasmaGrandierJaeger* pOwner) override;
	virtual void Update(CMiasmaGrandierJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaGrandierJaeger* pOwner) override;

private:
	_float m_Duration = {};
	_float m_AppearTime = {};

public:
	static CMiasmaJaeger_Appear* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaJaeger_Attack : public IHState<CMiasmaGrandierJaeger>
{
public:
	virtual void Enter(CMiasmaGrandierJaeger* pOwner) override;
	virtual void Update(CMiasmaGrandierJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaGrandierJaeger* pOwner) override;

public:
	static CMiasmaJaeger_Attack* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaJaeger_Hit : public IHState<CMiasmaGrandierJaeger>
{
public:
	virtual void Enter(CMiasmaGrandierJaeger* pOwner) override;
	virtual void Update(CMiasmaGrandierJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaGrandierJaeger* pOwner) override;

public:
	static CMiasmaJaeger_Hit* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaJaeger_DisAppear : public IHState<CMiasmaGrandierJaeger>
{
public:
	virtual void Enter(CMiasmaGrandierJaeger* pOwner) override;
	virtual void Update(CMiasmaGrandierJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaGrandierJaeger* pOwner) override;

public:
	static CMiasmaJaeger_DisAppear* Create();
	virtual void Free() override { __super::Free(); }
};

NS_END