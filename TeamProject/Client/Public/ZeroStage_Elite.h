#pragma once
#include "Stage.h"

NS_BEGIN(Client)

class CZeroStage_Elite :
    public CStage
{
private:
	CZeroStage_Elite();
	~CZeroStage_Elite() DEFAULT;

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
	static CZeroStage_Elite* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

NS_END