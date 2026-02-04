#pragma once
#include "Stage.h"
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
	void Battle(); 
	void Outro();
	void End();

private:
	_float3 baseColor= { 1.0f, 1.0f, 1.0f};
public:
	static CZeroStage_Elite* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();

};

