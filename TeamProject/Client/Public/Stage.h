#pragma once
#include "Base.h"
#include "Zero_Level.h"
NS_BEGIN(Client)
class CStage :
    public CBase
{
protected:
    CStage();
    ~CStage() DEFAULT;

public:
	virtual HRESULT Awake()PURE;
	virtual void    Update()PURE;

public:
	virtual HRESULT Ready_Stage(CZero_Level::StageContext& context)PURE;
	virtual HRESULT Enter_Stage(CZero_Level::StageContext& context)PURE;
	virtual HRESULT Exit_Stage(CZero_Level::StageContext& context)PURE;

protected:
	class CZero_Level* m_pOwnerLevel = { nullptr };
public:
    virtual void Free();
};
NS_END