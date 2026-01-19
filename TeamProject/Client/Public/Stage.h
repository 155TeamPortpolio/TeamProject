#pragma once
#include "Base.h"
#include "Zero_Level.h"
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
	class CZero_Level* m_pOwnerLevel = { nullptr };
	StageState m_eStageStage = {StageState::None };
public:
    virtual void Free();
};
NS_END