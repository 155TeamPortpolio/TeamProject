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
	// Ray Cast
	virtual _bool Raycast(const PHYSICS_RAY& desc, PHYSICS_RAY_HIT& outHit) PURE;
	virtual _bool Raycast_Multiple(const PHYSICS_RAY& desc, PHYSICS_RAY_HITS& outHits) PURE;
	virtual _bool Raycast_All(const PHYSICS_RAY& desc, PHYSICS_RAY_HITS& outHits) PURE;
	// Mesh Cooking
	virtual PxTriangleMesh* Cook_TriangleMesh(const string& strModelKey, class CModel* pModel) PURE;
};

NS_END
