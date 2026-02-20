#pragma once
#include "Stage.h"

NS_BEGIN(Client)

class CZeroStage_Rest :
	public CStage
{
private:
	CZeroStage_Rest();
	~CZeroStage_Rest() DEFAULT;

public:
	virtual HRESULT Initialize(class CZero_Level* pOwnerLevel);
	virtual HRESULT Awake() override;
	virtual void    Update()override;

public:
	virtual HRESULT Enter_Stage(StageContext& context)override;

private:
	void Intro();
	void Outro();
	void End();

private:
	virtual void Active_Portal() override;

public:
	static CZeroStage_Rest* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

NS_END