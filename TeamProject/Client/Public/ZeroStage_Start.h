#pragma once
#include "Stage.h"
class CZeroStage_Start :
	public CStage
{
private:
	CZeroStage_Start();
	~CZeroStage_Start() DEFAULT;

public:
	virtual HRESULT Initialize(class CZero_Level* pOwnerLevel);
	virtual HRESULT Awake() override;
	virtual void    Update()override;

public:
	virtual HRESULT Enter_Stage(StageContext& context)override;

private:
	void Intro();
	void Battle();
	void Outro();
	void End();
private:

public:
	static CZeroStage_Start* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

