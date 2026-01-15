#pragma once
#include "Stage.h"

NS_BEGIN(Client)
class CZeroStage_1_1 :
	public CStage
{
private:
	CZeroStage_1_1();
	~CZeroStage_1_1() DEFAULT;

public:
	virtual HRESULT Initialize(class CZero_Level* pOwnerLevel);
	virtual HRESULT Awake() override;
	virtual void    Update()override;

public:
	virtual HRESULT Ready_Stage(CZero_Level::StageContext& context)override;
	virtual HRESULT Enter_Stage(CZero_Level::StageContext& context)override;
	virtual HRESULT Exit_Stage(CZero_Level::StageContext& context)override;

private:
	void Ready_Map(const string& LevelTag, const string& AreaTag);
	void Rake_MapResources();

public:
	static CZeroStage_1_1* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

NS_END