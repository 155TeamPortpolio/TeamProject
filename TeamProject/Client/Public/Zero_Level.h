#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance; 
NS_END

NS_BEGIN(Client)
class CCamDirector;

struct StageNode {
	StageType		MyType;
	_int			ParentIndex;
	vector<_int>	ChildrenIndex; 
	_int depth = 0;
	_bool visited = { false };    
};

struct MapCycle
{
	vector<string> maps;
	_int cursor = 0; /*ÇöÀç*/

	string Next()
	{
		if (maps.empty()) return {};
		if (cursor < 0) cursor = 0;
		string key = maps[cursor % (int)maps.size()];
		cursor = (cursor + 1) % (int)maps.size();
		return key;
	}
};

typedef struct tagStageContext {
	_bool isFirstIn = false;
	OBJECT_HANDLE hPlayer;
	CStage* pNowStage = { nullptr };
	StageType nextType = StageType::Normal;
	string mapKey;
}StageContext;

class CZero_Level : public CLevel
{
public:


private:
	CZero_Level(const string& LevelKey);
	virtual ~CZero_Level() DEFAULT;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Awake()      override;
	virtual void    Update()     override;
	virtual HRESULT Render()     override;

public:
	StageContext& Get_StageContext() { return m_Context; };
	HRESULT ChangeStage(StageType type = StageType::Normal);
	string PopMapKey(StageType type);

	class CStageRouter* Get_Router() { return m_pRouter; }

private:
	void Ready_Prototype();
	void Ready_Stage();

private:
	StageContext m_Context = {};
	class CStageRouter* m_pRouter = { nullptr };
	unordered_map<StageType,class CStage*> m_StageContainer;
	unordered_map<StageType, MapCycle> m_mapCycle;

public:
	static CZero_Level* Create(const string& LevelKey);
	virtual void Free() override;
};

NS_END