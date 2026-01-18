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
	virtual HRESULT Ready_Stage(CZero_Level::StageContext& context)override;
	virtual HRESULT Enter_Stage(CZero_Level::StageContext& context)override;
	virtual HRESULT Exit_Stage(CZero_Level::StageContext& context)override;

private:
	void Intro();
	void Battle(); 
	void Outro();
private:
	OBJECT_HANDLE m_PlayerHandle = {};
	_bool m_isSequenceEnd = {};
	_float m_radialDt = {};
public:
	static CZeroStage_Elite* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

