#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCameraTrail final : public CComponent
{
private:
	CCameraTrail() = default;
	CCameraTrail(const CCameraTrail& rhs) : CComponent(rhs) {}
	~CCameraTrail() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:



public:
	static CCameraTrail* Create();
	virtual void Free() override;
	CComponent* Clone() override;
};

NS_END