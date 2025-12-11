#pragma once
#include "IService.h"

NS_BEGIN(Engine)

class ENGINE_DLL IPhysicsService abstract : public IService
{
protected:
	virtual  ~IPhysicsService() DEFAULT;

public:
	virtual void Update(_float dt) PURE;
	virtual void Late_Update(_float dt) PURE;

};

NS_END
