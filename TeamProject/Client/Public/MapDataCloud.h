#pragma once
#include "Base.h"

NS_BEGIN(Client)

class CMapDataCloud final : public CBase
{
private:
	CMapDataCloud();
	virtual ~CMapDataCloud() = default;


public:
	static CMapDataCloud* Create();
	virtual void Free() override;
};

NS_END