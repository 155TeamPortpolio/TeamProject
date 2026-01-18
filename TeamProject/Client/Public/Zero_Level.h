#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance; 
NS_END

NS_BEGIN(Client)
class CCamDirector;
class CZero_Level : public CLevel
{
public:
	enum class StageType {Normal, Elite, Boss};
	typedef struct tagStageContext {
		StageType eStageType;
		_int StageID = { -1 };
		class CStage* pNowStage = { nullptr };
		OBJECT_HANDLE hPlayer = {};
	}StageContext;

private:
	CZero_Level(const string& LevelKey);
	virtual ~CZero_Level() DEFAULT;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Awake()      override;
	virtual void    Update()     override;
	virtual HRESULT Render()     override;

public:
	static void PreLoad_Level();
	StageContext& Get_StageContext() { return m_Context; };
	HRESULT ChangeStage(StageType nextStageType, _int StageID);

private:
	CGameInstance* m_pGameInstance = {nullptr};
	CCamDirector* m_pCamDirector = { nullptr };
	StageContext m_Context = {};
	unordered_map<StageType,class CStage*> m_StageContainer;

public:
	static CZero_Level* Create(const string& LevelKey);
	virtual void Free() override;
};

NS_END