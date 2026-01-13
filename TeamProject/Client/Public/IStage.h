#pragma once
#include "Base.h"
NS_BEGIN(Client)
class IStage :
    public CBase
{
protected:
    IStage();
    ~IStage() DEFAULT;

public:
	virtual HRESULT Awake()PURE;
	virtual void    Update()PURE;
	virtual HRESULT Render()PURE;

public:
	virtual HRESULT Ready_Stage()PURE;
	virtual HRESULT Enter_Stage()PURE;
	virtual HRESULT Exit_Stage()PURE;

public:
    virtual void Free();
};
NS_END