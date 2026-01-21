#pragma once
#include "Base.h"
#include "Zero_Level.h"
#include "StageFx.h"

NS_BEGIN(Client)
class CStage :
    public CBase
{
protected:
	enum class StageState {None,Entrance,BattleStart,BattleEnd,Outro,End};

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

	virtual void StageChangeOn(CZero_Level::StageType nextStageType, _int StageID);

protected:
	virtual void Ready_Map(const string& LevelTag, const string& AreaTag);

protected:
	void BaseIntro(CZero_Level::StageContext& context);
	void BossIntro(CZero_Level::StageContext& context);
	void BaseOutro();

protected:
	_float m_fStageTime = {};
	class CZero_Level* m_pOwnerLevel = { nullptr };
	StageState m_eStageStage = {StageState::None };
	OBJECT_HANDLE m_PlayerHandle = {};


	EffectFlow m_introFlow;
	_bool m_introFlowBuilt = false;
	EffectFlow m_outroFlow;
	_bool m_outroFlowBuilt = false;
public:
    virtual void Free();
};
NS_END