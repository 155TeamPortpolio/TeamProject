#pragma once
#include "Base.h"
#include "Zero_Level.h"
#include "StageFx.h"
#include "MapData_Defines.h"

NS_BEGIN(Client)
class CStage :
    public CBase
{
protected:
	enum class StageState {None,Entrance,BattleStart,BattleEnd,Outro,End};
	struct StageContext{
		StageType eStageType;
		_int StageID = { -1 };
		vector<BATTLEOBJ_INFO> player;
		vector<BATTLEOBJ_INFO> monster;
		vector<BATTLEOBJ_INFO> portal;
	};
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

	virtual void StageChangeOn(StageType nextStageType, _int StageID);

protected:
	virtual void Ready_Map(const string& LevelTag, const string& AreaTag);

protected:
	void BaseIntro(CZero_Level::StageContext& context);
	void BossIntro(CZero_Level::StageContext& context);
	void BaseOutro();

private:
	HRESULT ReadyPlayerPoint(const  vector<BATTLE_POINT_DATA>& point);
	HRESULT ReadyPortalPoint(const  vector<BATTLE_POINT_DATA>& point);
	HRESULT ReadyMonsterPoint(const vector<BATTLE_POINT_DATA>& point);
	HRESULT ReadyMonsterData(const string& LevelTag, const string& AreaTag);

protected:
	_float m_fStageTime = {};
	class CZero_Level* m_pOwnerLevel = { nullptr };

	StageState m_eStageStage = {StageState::None };
	OBJECT_HANDLE m_PlayerHandle = {};


	EffectFlow m_introFlow;
	_bool m_introFlowBuilt = false;
	EffectFlow m_outroFlow;
	_bool m_outroFlowBuilt = false;

	StageContext m_Context;
protected:

public:
    virtual void Free();
};
NS_END