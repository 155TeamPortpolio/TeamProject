#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance; 
NS_END

NS_BEGIN(Client)
class CMainCity_Level : public CLevel
{
private:
	CMainCity_Level(const string& LevelKey);
	virtual ~CMainCity_Level() DEFAULT;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Awake()      override;
	virtual void    Update()     override;
	virtual HRESULT Render()     override;

public:
	static void PreLoad_Level();

private:
	void ReadyMap();
	void ReadyCamera();
	void ReadyShadowCamera();
private:
	CGameInstance* m_pGameInstance={nullptr};
	class CCamDirector* m_pCamDirector = { nullptr };
	class CMapDataCloud* m_pMapDataCloud = { nullptr };

public:
	static CMainCity_Level* Create(const string& LevelKey);
	virtual void Free() override;
};

NS_END