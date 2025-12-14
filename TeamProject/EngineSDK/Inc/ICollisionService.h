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

#ifdef _DEBUG
   virtual _int RegisterCollider(class CCollider* pCollider, _int Index) PURE;
   virtual void UnregisterCollider(class CCollider* pCollider, _int Index)PURE;
   virtual void Render_Debug() PURE;
#endif 
};
NS_END
