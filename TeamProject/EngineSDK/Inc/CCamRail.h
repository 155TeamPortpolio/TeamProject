#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamRail final : public CComponent
{
private:
	CCamRail() = default;
	CCamRail(const CCamRail& rhs) : CComponent(rhs) {}
	~CCamRail() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:



public:
	static CCamRail* Create();
	virtual void Free() override;
	CComponent* Clone() override;
};

NS_END