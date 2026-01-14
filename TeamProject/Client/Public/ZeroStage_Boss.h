#pragma once
#include "Stage.h"

NS_BEGIN(Client)
class CZeroStage_Boss :
    public CStage
{
private:
	CZeroStage_Boss();
	~CZeroStage_Boss() DEFAULT;

public:
	virtual HRESULT Initialize(class CZero_Level* pOwnerLevel);
	virtual HRESULT Awake() override;
	virtual void    Update()override;

public:
	virtual HRESULT Ready_Stage(CZero_Level::StageContext& context)override;
	virtual HRESULT Enter_Stage(CZero_Level::StageContext& context)override;
	virtual HRESULT Exit_Stage(CZero_Level::StageContext& context)override;

private:

public:
	static CZeroStage_Boss* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

NS_END