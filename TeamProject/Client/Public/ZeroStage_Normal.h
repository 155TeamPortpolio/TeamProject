#pragma once
#include "Stage.h"

NS_BEGIN(Client)

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
	virtual HRESULT Enter_Stage(StageContext& context)override;

private:
	void Intro();
	void BattleStart();
	void Battle(); 
	void BattleEnd(); 
	void Outro();
	void End();

public:
	static CZeroStage_Normal* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

NS_END