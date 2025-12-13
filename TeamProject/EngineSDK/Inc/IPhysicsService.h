#pragma once
#include "IService.h"

NS_BEGIN(Engine)

class ENGINE_DLL IPhysicsService abstract : public IService
{
protected:
	virtual  ~IPhysicsService() DEFAULT;

public:
	virtual PxPhysics*		     Get_Physics() PURE;
	virtual PxScene*			 Get_Scene() PURE;
	virtual PxControllerManager* Get_ControllerManager()PURE;
	virtual PxMaterial*			 Get_DefaultMaterial() PURE;
	virtual HRESULT              Add_Material(const string& strKey, _float fStatic, _float fDynamic, _float fRestitution) PURE;
	virtual PxMaterial*			 Get_Material(const string& strKey) PURE;

public:
	virtual void Update(_float dt) PURE;
	virtual void Late_Update(_float dt) PURE;

};

NS_END
