#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_ChangePhase : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_ChangePhase* Create() { return new CSacrificeState_ChangePhase(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrifice* pOwner);
	void Update_RimLightColor(CSacrifice* pOwner);

	_float m_fRimLightChangeDuration = 1.f;
	_float3 m_vStartColor{ 1.f,0.3f,0.f };
	_float3 m_vEndColor{ 1.f,0.f,0.f };
};
NS_END
