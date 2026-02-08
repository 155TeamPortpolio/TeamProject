#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiasmaJaeger;
class CMiasmaJaeger_Appear : public IHState<CMiasmaJaeger>
{
public:
	virtual void Enter(CMiasmaJaeger* pOwner) override;
	virtual void Update(CMiasmaJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaJaeger* pOwner) override;

private:
	_float m_Duration = {};
	_float m_AppearTime = {};

public:
	static CMiasmaJaeger_Appear* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaJaeger_Attack : public IHState<CMiasmaJaeger>
{
public:
	virtual void Enter(CMiasmaJaeger* pOwner) override;
	virtual void Update(CMiasmaJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaJaeger* pOwner) override;

public:
	static CMiasmaJaeger_Attack* Create();
	virtual void Free() override { __super::Free(); }
};

class CMiasmaJaeger_DisAppear : public IHState<CMiasmaJaeger>
{
public:
	virtual void Enter(CMiasmaJaeger* pOwner) override;
	virtual void Update(CMiasmaJaeger* pOwner, _float dt) override;
	virtual void Exit(CMiasmaJaeger* pOwner) override;

public:
	static CMiasmaJaeger_DisAppear* Create();
	virtual void Free() override { __super::Free(); }
};

NS_END