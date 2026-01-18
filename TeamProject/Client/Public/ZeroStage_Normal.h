#pragma once
#include "Stage.h"
class CZeroStage_Normal :
    public CStage
{
private:
	CZeroStage_Normal();
	~CZeroStage_Normal() DEFAULT;

public:
	virtual HRESULT Initialize(class CZero_Level* pOwnerLevel);
	virtual HRESULT Awake() override;
	virtual void    Update()override;

public:
	virtual HRESULT Ready_Stage(CZero_Level::StageContext& context)override;
	virtual HRESULT Enter_Stage(CZero_Level::StageContext& context)override;
	virtual HRESULT Exit_Stage(CZero_Level::StageContext& context)override;

public:
	static CZeroStage_Normal* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

