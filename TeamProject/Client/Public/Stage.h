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
	enum class StageState	{None,Entrance,BattleStart,BattleEnd,Outro,End};
	enum class PlayerPoint	{Typical,Sub,End};
	struct Combined_MonsterData
	{
		struct creation {
			MonsterCreationDesc creationInfo;
			_int Count = {};
		};
		vector<creation> CreationData;
		vector<_float3>	 SpawnPoint;
		void Reset() {
			vector<creation> dummy;
			vector<_float3> dummy2;
			CreationData.swap(dummy);
			SpawnPoint.swap(dummy2);
		}
	};

	struct PlayerSRT
	{
		_float4 pos;
		_float3 rotation;
	};

protected:
    CStage();
    ~CStage() DEFAULT;

public:
	virtual HRESULT Awake()PURE;
	virtual void    Update()PURE;

public:
	virtual HRESULT Enter_Stage(StageContext& context)PURE;
	virtual HRESULT Exit_Stage(StageContext& context);
	virtual void StageChangeOn(_int choiceIndex);

protected:
	virtual void Ready_Map(const string& LevelTag, const string& AreaTag);
	virtual void Reserve_Enemy(const string& LevelTag);
	virtual void Active_Enemy();
	virtual void Active_Player(PlayerPoint pointType);
	virtual void Active_Portal();

protected:
	void BaseIntro(StageContext& context);
	void BossIntro(StageContext& context);
	void BaseOutro();

private:
	HRESULT ReadyPlayerPoint(const  vector<BATTLE_POINT_DATA>& point);
	HRESULT ReadyPortalPoint(const  vector<BATTLE_POINT_DATA>& point);
	HRESULT ReadyMonsterPoint(const vector<BATTLE_POINT_DATA>& point);
	HRESULT ReadyMonsterData(const string& LevelTag, const string& AreaTag);

protected:
	class CZero_Level* m_pOwnerLevel = { nullptr };
	StageState	m_eStageStage = {StageState::None };
	StageType	m_eType = {};
	_int m_iNextChoice = { -1 };

	/*연출*/
	EffectFlow m_introFlow;
	_bool m_introFlowBuilt = false;
	EffectFlow m_outroFlow;
	_bool m_outroFlowBuilt = false;

	/*데이터 - 몬스터*/
	Combined_MonsterData m_MonsterData = {};
	vector<class CGameObject*> m_pMonsters;

	/*데이터 - 플레이어*/
	OBJECT_HANDLE m_PlayerHandle = {};
	array<PlayerSRT, ENUM(PlayerPoint::End)> m_PlayerPoint;

	/*데이터 - 스테이지 포탈*/
	vector<class CGameObject*> m_pPortals;

	/*데이터 - 맵 유형 개수*/
public:
    virtual void Free();
};
NS_END
