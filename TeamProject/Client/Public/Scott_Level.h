#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance; 
NS_END

NS_BEGIN(Client)
class CScott_Level : public CLevel
{
private:
	CScott_Level(const string& LevelKey);
	virtual ~CScott_Level() DEFAULT;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Awake()      override;
	virtual void    Update()     override;
	virtual HRESULT Render()     override;

public:
	static void PreLoad_Level();

private:
	void Ready_Map(const string& LevelTag, const string& AreaTag);
	void Rake_MapResources();

private:
	CGameInstance* m_pGameInstance{};

public:
	static CScott_Level* Create(const string& LevelKey);
	virtual void Free() override;
};

NS_END