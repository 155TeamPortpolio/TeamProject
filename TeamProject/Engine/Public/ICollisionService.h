#pragma once
#pragma once
#include "IService.h"
NS_BEGIN(Engine)
class ENGINE_DLL ICollisionService abstract :
public IService
{
protected:
    virtual  ~ICollisionService() DEFAULT;
public:
    virtual void Update(_float dt) PURE;
    virtual void Late_Update(_float dt) PURE;
    virtual void Render_GUI() PURE;
    virtual PxUserControllerHitReport* Get_CCTCallback() PURE;
    virtual _int RegisterCollidable(class ICollidable* pCollidable, _int Index) PURE;
    virtual void UnRegisterCollidable(class ICollidable* pCollidable, _int Index) PURE;
   virtual void  Set_Render(_bool bRender) PURE;
   virtual _bool Get_Render() const PURE;

#ifdef _DEBUG
    virtual void Render_Debug() PURE;
#endif 
};
NS_END
