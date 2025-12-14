#pragma once
#pragma once
#include "IService.h"
NS_BEGIN(Engine)
class ENGINE_DLL ILightService abstract :
public IService
{
protected:
	virtual  ~ILightService() DEFAULT;
public:
    virtual _int Register_Light(class CLight* Light, _int Index)PURE;
    virtual void UnRegister_Light(class CLight* Light, _int Index)PURE;
    virtual void DeActive_Light(class CLight* Light, _int Index)PURE;
    virtual void Active_Light(class CLight* Light, _int Index)PURE;
    virtual vector<LIGHT_DESC> Visible_Lights() PURE;

};
NS_END
