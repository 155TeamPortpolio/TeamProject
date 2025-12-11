#pragma once
#include "IService.h"
NS_BEGIN(Engine)
class ENGINE_DLL IRayService abstract :
public IService
{
protected:
	virtual  ~IRayService() DEFAULT;
public:
	virtual void Update(_float dt) PURE;
	virtual _int Register_RayReceiver(class CRayReceiver* RayTarget) PURE;
	virtual void UnRegister_RayReceiver(_int ID) PURE;
	virtual void Register_Ray(RAY* ray) PURE;
	virtual void Delete_Ray() PURE;
	virtual RAY_HIT* Get_FrontRayHit() PURE;

};
NS_END
